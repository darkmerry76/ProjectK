#include "EMSheetBuilderTransform.h"
#include "Config/EMTableConfig.h"
#include "DataTable/EMDataTable.h"
#include "Parser/EMCppParser.h"
#include "SheetBuilder/EMSheetBuilder.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "Runtime/CoreUObject/Public/UObject/MetaData.h"
#include "CoreUObject/Public/UObject/EnumProperty.h"
#include "CoreUObject/Public/UObject/Package.h"
#include "CoreUObject/Public/UObject/NoExportTypes.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMDataTableListener
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSet<FName> FEMDataTableListener::RowKeys;

FEMDataTableListener::FEMDataTableListener(UScriptStruct* RowStruct, TMap<FName, uint8*>* InRowMap) :
	EMTableCore::FEMRowStructVariableListener(RowStruct, InRowMap)
{
}

void FEMDataTableListener::AddRowInternal(FName RowName, uint8* RowDataPtr)
{
	if(nullptr != RowMap->Find(RowName) || nullptr != RowKeys.Find(RowName))
	{
		if (true == EMTableConfig::Get().GetGenericSection().CheckKeyOverlap)
		{
			EMTableGrammar::CurrentColumn = 0;
			EMTableGrammar::CurrentRow = RowMap->Num() + 3;
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Key_Exist,
				FString::Printf(TEXT("겹치는 키값('%s')을 찾았습니다. 키값은 겹치지 않게 해주세요. Enum은 Enum멤버값이 겹치면 중복된키로 취급합니다."), *RowName.ToString()), EMTableCore::EEMSyntexException::Error);
		}
	}
	else {
		RowMap->Emplace(RowName, RowDataPtr);
		RowKeys.Emplace(RowName);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CreateProjectPackage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UPackage* CreateProjectPackage()
{
	return CreatePackage(*FString::Printf(TEXT("/Script/%s"), *EMTableConfig::Get().GetUAssetExportSection().Project));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderTransform
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetBuilderTransform::EMSheetBuilderTransform()
{

}

EMSheetBuilderTransform::~EMSheetBuilderTransform()
{
	Clear();
}

void EMSheetBuilderTransform::Clear()
{
	DataTables.Empty();
	UnrealEnums.Empty();
}

void EMSheetBuilderTransform::AddFieldProperty(UScriptStruct* ScriptStruct, const EMTableCore::FEMVariable& Variable, bool AddElementSize)
{
	class FImpProperty : public FProperty
	{
	public:
		void SetImpOffset_Internal(int32 NewOffset)
		{
			SetOffset_Internal(NewOffset);
		}
		void ImpInit()
		{
			Init();
		}
	};

	FName PropertyName = (EMTableCore::EEMLinkVariableType::Table == Variable.LinkType) ?
		*FString::Printf(TEXT("%s_Private"), *Variable.Name) : *Variable.Name;
	
	const EObjectFlags ObjectFlags = RF_Public | RF_Transactional | RF_WasLoaded | RF_LoadCompleted;

	FArrayProperty* ArrayProperty = nullptr;
	FFieldVariant Owner = ScriptStruct;
	if (true == Variable.IsArray)
	{
		ArrayProperty = CastField<FArrayProperty>(FField::Construct(FArrayProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		Owner = FFieldVariant(ArrayProperty);
	}

	FImpProperty* ResultProperty = nullptr;
	switch (Variable.Type)
	{
	case EMTableCore::EEMVariabaleType::Boolean:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FBoolProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::Uint32:
	case EMTableCore::EEMVariabaleType::Uint16:
	case EMTableCore::EEMVariabaleType::Uint8:
	case EMTableCore::EEMVariabaleType::Int32:
	case EMTableCore::EEMVariabaleType::Int16:
	case EMTableCore::EEMVariabaleType::Int8:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FIntProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;

	case EMTableCore::EEMVariabaleType::Int64:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FInt64Property::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::Uint64:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FUInt64Property::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::Float:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FFloatProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::Double:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FDoubleProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::Enum:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FEnumProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		{
			FEnumProperty* EnumProperty = CastField<FEnumProperty>(ResultProperty);
			if (nullptr != EnumProperty)
			{
				FNumericProperty* UnderlyingProp = nullptr;
				switch (Variable.ExtraType)
				{
				case EMTableCore::EEMEnumVariableType::Int8: UnderlyingProp = new FInt8Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Uint8: UnderlyingProp = new FByteProperty(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Int16: UnderlyingProp = new FInt16Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Uint16: UnderlyingProp = new FUInt16Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Int32:UnderlyingProp = new FIntProperty(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Uint32:UnderlyingProp = new FUInt32Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Long:UnderlyingProp = new FIntProperty(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Ulong: UnderlyingProp = new FUInt32Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Long_Long:UnderlyingProp = new FInt64Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Int64:UnderlyingProp = new FInt64Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Uint64:UnderlyingProp = new FUInt64Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				case EMTableCore::EEMEnumVariableType::Ulong_Ulong: UnderlyingProp = new FUInt64Property(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				default: UnderlyingProp = new FByteProperty(EnumProperty, TEXT("UnderlyingType"), RF_Public); break;
				}
				EnumProperty->AddCppProperty(UnderlyingProp);

				((FImpProperty*)UnderlyingProp)->SetElementSize(EMTableCore::GetEnumVariableInfo(Variable.ExtraType).Stride);

				EnumProperty->SetElementSize(EMTableCore::GetEnumVariableInfo(Variable.ExtraType).Stride);

				UEnum** FindEnum = UnrealEnums.Find(Variable.ExtraName);
				if (nullptr != FindEnum)
				{
					EnumProperty->SetEnum(*FindEnum);
				}
			}
		}
		break;
	case EMTableCore::EEMVariabaleType::PdaKey:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStrProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::StringToClass:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStrProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::StringToObject:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStrProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::JsonStruct:
	{
		TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(*Variable.ExtraName.Replace(TEXT("[]"), TEXT("")));
		ensure(MemberStructure.IsValid());
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStructProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(ResultProperty);
			if (nullptr != StructProperty)
			{
				StructProperty->Struct = MemberStructure->GetUStruct();
				StructProperty->SetElementSize(MemberStructure->GetElementSize());
			}
		}
		break;
	}
	break;
	case EMTableCore::EEMVariabaleType::String:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStrProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));		
		break;
	case EMTableCore::EEMVariabaleType::Name:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FNameProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		break;
	case EMTableCore::EEMVariabaleType::Color:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStructProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(ResultProperty);
			if (nullptr != StructProperty)
			{
				StructProperty->Struct = FEMColor::StaticStruct();
				StructProperty->SetElementSize(sizeof(FColor));
			}
		}
		break;
	case EMTableCore::EEMVariabaleType::Vector2:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStructProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(ResultProperty);
			if (nullptr != StructProperty)
			{
				StructProperty->Struct = FEMVector2D::StaticStruct();
				StructProperty->SetElementSize(sizeof(FEMVector2D));
			}
		}
		break;
	case EMTableCore::EEMVariabaleType::Vector3:
		ResultProperty = CastField<FImpProperty>(FField::Construct(FStructProperty::StaticClass()->GetFName(), Owner, PropertyName, ObjectFlags));
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(ResultProperty);
			if (nullptr != StructProperty)
			{
				StructProperty->Struct = FEMVector::StaticStruct();
				StructProperty->SetElementSize(sizeof(FEMVector));
			}
		}
		break;

	default:
		EM_LOG(EMLog, Fatal, TEXT("정의되지않은 변수타잎입니다.[%d]!!!"), (int32)Variable.Type);
	}

	check(nullptr != ResultProperty);

	ResultProperty->SetPropertyFlags(CPF_Edit | CPF_BlueprintVisible | CPF_BlueprintReadOnly |
		CPF_ZeroConstructor | CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic);

	if (true == Variable.IsArray)
	{
		ArrayProperty->SetPropertyFlags(CPF_Edit | CPF_BlueprintVisible | CPF_BlueprintReadOnly |
			CPF_ZeroConstructor | CPF_HasGetValueTypeHash | CPF_NativeAccessSpecifierPublic);

		ArrayProperty->Inner = ResultProperty;
		ResultProperty = CastField<FImpProperty>(ArrayProperty);
	}

	ResultProperty->SetImpOffset_Internal(ScriptStruct->PropertiesSize);

	ScriptStruct->AddCppProperty(ResultProperty);

	ResultProperty->PropertyLinkNext = ScriptStruct->PropertyLink;
	ScriptStruct->PropertyLink = ResultProperty;

	if(true == AddElementSize)
	{
		ScriptStruct->PropertiesSize += ResultProperty->GetElementSize();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CreateUAsset
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UScriptStruct* EMSheetBuilderTransform::CreateUStruct(TSharedPtr<FEMStructure> Structure)
{
	int32 ElementSize = Structure->GetElementSize();
	if(ElementSize == 0)
	{
		ElementSize = 1;
	}
	int32 AlignSize = Structure->GetAlignSize();
	if (AlignSize == 0)
	{
		AlignSize = 1;
	}
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Core/KMDefine.h" },
	};
	const UECodeGen_Private::FStructParams ReturnStructParams = {
		(UObject * (*)())CreateProjectPackage,
		nullptr,
		nullptr,
		TCHAR_TO_ANSI(*FEMStructure::GetClassNameUE4(Structure.Get())),
		0,
		0,
		0,
		AlignSize,
		RF_Public | RF_Transient | RF_MarkAsNative,
		EStructFlags(0x00000000),
		METADATA_PARAMS(UE_ARRAY_COUNT(Struct_MetaDataParams), Struct_MetaDataParams)
	};

	UScriptStruct* ResultStruct = nullptr;
	UECodeGen_Private::ConstructUScriptStruct(ResultStruct, ReturnStructParams);

	if (nullptr == ResultStruct)
	{
		EM_LOG(EMLog, Fatal, TEXT("UStruct \"%s\" 생성에 실패했습니다."), *Structure->GetName().ToString());
	}
	
	return ResultStruct;
}

void EMSheetBuilderTransform::CreateUStructProperties(TSharedPtr<FEMStructure> Structure)
{	
	if (false == Structure.IsValid() || false == IsValid(Structure->GetUStruct()))
	{
		EM_LOG(EMLog, Fatal, TEXT("UStruct \"%s\" 생성에 실패했습니다."), *Structure->GetName().ToString());
		return;
	}
	UScriptStruct* ScriptStruct = Structure->GetUStruct();
	for (int32 i = Structure->GetMemberCount() - 1; i >= 0; --i)
	{
		TSharedPtr<FEMStructureMember> StructureMember = Structure->GetMember(i);

		EMTableCore::FEMVariable TempVariable;
		TempVariable.Name = StructureMember->GetName().ToString();
		TempVariable.Type = StructureMember->GetTableVariableType();
		TempVariable.ExtraName = StructureMember->GetType().ToString();

		if(StructureMember->GetTableVariableType() == EMTableCore::EEMVariabaleType::Enum)
		{
			TempVariable.ExtraType = EMTableCore::EEMEnumVariableType::Uint8;
			const TSharedPtr<EMCppEnumNode> EnumNode = EMCppParser::Get().FindEnum(*TempVariable.ExtraName);
			if (true == EnumNode.IsValid())
			{
				TempVariable.ExtraType = EnumNode->VariableType;
			}
		}
		TempVariable.IsArray = StructureMember->IsArray();

		AddFieldProperty(ScriptStruct, TempVariable);
	}
}

UDataTable* EMSheetBuilderTransform::CreateUAsset(UPackage* DataPackage, EMTableCore::FEMTableBinaryReader& Reader, const TSharedPtr<EMSheetBuilder>& SheetBuilder)
{
	double UassetPrevTime = FPlatformTime::Seconds();

	FString StructureName = FString::Printf(TEXT("%sRow"), *SheetBuilder->GetSheetName());

	UDataTable* DataTable = NewObject<UDataTable>(DataPackage, *SheetBuilder->GetSheetName(), RF_Public | RF_Standalone | RF_Transactional);

	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Core/KMDefine.h" },
	};

	const UECodeGen_Private::FStructParams ReturnStructParams = {
		(UObject * (*)())CreateProjectPackage,
		nullptr,
		nullptr,
		TCHAR_TO_ANSI(*StructureName),
		0,
		0,
		0,
		alignof(FTableRowBase),
		RF_Public | RF_Transient | RF_MarkAsNative,
		EStructFlags(0x00000000),
		METADATA_PARAMS(UE_ARRAY_COUNT(Struct_MetaDataParams), Struct_MetaDataParams)
	};
	
	UScriptStruct* ResultStruct = nullptr;
	UECodeGen_Private::ConstructUScriptStruct(ResultStruct, ReturnStructParams);

	if (nullptr == ResultStruct)
	{
		EM_LOG(EMLog, Fatal, TEXT("UDataTable \"%s\" TableRowBase 생성에 실패했습니다."), *SheetBuilder->GetSheetName());
	}

	for (int32 i = SheetBuilder->GetWorldVariables().Num() - 1; i >= 0; i--)
	{
		AddFieldProperty(ResultStruct, SheetBuilder->GetWorldVariables()[i]);
	}

	DataTable->RowStruct = ResultStruct;

	TSharedPtr<EMTableCore::FEMTableSheetHeader> SheetHeader = MakeShareable(new EMTableCore::FEMTableSheetHeader);
	SheetHeader->SheetName = SheetBuilder->GetSheetName();
	SheetHeader->ColumnCount = (int16)SheetBuilder->GetWorldVariables().Num();
	SheetHeader->RowCount = SheetBuilder->GetMaxRowCount();
	SheetHeader->SeekAddress = 0;
	SheetHeader->CompressLength = 0;
	SheetHeader->DataLength = SheetBuilder->GetDataLength();

	FEMDataTableListener Listener(DataTable->RowStruct, &DataTable->GetNonConstRowMap());
	Reader.ReadSheet(SheetHeader, &Listener);

	EM_LOG(EMLog, Display, TEXT("UDataTable \"%s\"로 변경 완료[%.4f]"), *SheetBuilder->GetSheetName(), FPlatformTime::Seconds() - UassetPrevTime);

	return DataTable;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TransformUDataTable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderTransform::DoTransform(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders)
{
	EM_LOG(EMLog, Display, TEXT("============ UDataTables[%d]로 변경 ============"), SheetBuilders.Num());

	UObject* DefaultObjectOuter = (UDataTable::StaticClass()->ClassDefaultObject != nullptr) ? UDataTable::StaticClass()->ClassDefaultObject->GetOuter() : nullptr;
	for (auto ItEnum = EMCppParser::Get().GetEnums().CreateConstIterator(); ItEnum; ++ItEnum)
	{
		const TSharedPtr<EMCppEnumNode>& CppEnumPtr = ItEnum->Value;
		if (false == CppEnumPtr.IsValid())
		{
			continue;
		}

		TArray<TPair<FName, int64>> NewCppToUnrealEnums;

		for (int32 j = 0; j < CppEnumPtr->Members.Num(); j++)
		{
			TSharedPtr<EMCppEnumMemberNode> CppEnumValuePtr = CppEnumPtr->Members[j];
			if (false == CppEnumValuePtr.IsValid())
			{
				continue;
			}
			NewCppToUnrealEnums.Emplace(CppEnumPtr->Name + TEXT("::") + CppEnumValuePtr->Name, CppEnumValuePtr->Value);
		}
		UEnum* UnrealEnum = NewObject<UEnum>(DefaultObjectOuter, *CppEnumPtr->Name, RF_Public | RF_Standalone | RF_Transactional);
		UnrealEnum->AddToRoot();
		UnrealEnum->SetEnums(NewCppToUnrealEnums, UEnum::ECppForm::EnumClass);
		UnrealEnums.Emplace(CppEnumPtr->Name, UnrealEnum);
	}

	for (auto StructureIt : FEMStructureManager::Inst().GetStructures())
	{
		if (false == StructureIt.Value.IsValid())
		{
			continue;
		}
		if (true == StructureIt.Value->IsTableStructure())
		{
			continue;
		}
		StructureIt.Value->SetUStruct(CreateUStruct(StructureIt.Value));
	}

	for (auto StructureIt : FEMStructureManager::Inst().GetStructures())
	{
		if (false == StructureIt.Value.IsValid())
		{
			continue;
		}
		if (true == StructureIt.Value->IsTableStructure())
		{
			continue;
		}
		CreateUStructProperties(StructureIt.Value);
	}

	struct FMetaDataPairParam
	{
		const char* NameUTF8;
		const char* ValueUTF8;
	};

	const FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintInternalUseOnly", "true" },
		{ "BlueprintType", "true" },
	};

	FString DataSheetPath;
	UPackage* DataPackage = nullptr;
	if (true == EMTableConfig::Get().GetUAssetExportSection().IsMerge)
	{
		DataSheetPath = FString::Printf(TEXT("%s%s"),
			*EMTableConfig::Get().GetUAssetExportSection().ContentsPath, *EMTableConfig::Get().GetUAssetExportSection().MergeFilename);
		DataPackage = CreatePackage(*DataSheetPath);
		DataPackage->GetMetaData();
	}

	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		if (false == SheetBuilders[i]->GetExport())
		{
			continue;
		}
		const EMSheetBuilder_MemoryFile* SheetBuilderMemoryFile = (EMSheetBuilder_MemoryFile*)SheetBuilders[i].Get();
		if (SheetBuilderMemoryFile->CastName() != "EMSheetBuilder_MemoryFile")
		{
			continue;
		}

		if (false == SheetBuilders[i].Get()->GetParent().IsValid() || SheetBuilders[i].Get()->GetParent().Pin().Get() == EMSheetBuilderMgr::Get().GetRootSheet().Get())
		{
			FEMDataTableListener::RowKeys.Empty();
		}
		EMTableGrammar::ResetExceptionInfo();

		for (int32 j = 0; j < SheetBuilders[i]->GetAnalizers().Num(); j++)
		{
			if (0 == j)
			{
				EMTableGrammar::CurrentExcelName = SheetBuilders[i]->GetAnalizers()[j]->GetExcelName();
			}
			else
			{
				EMTableGrammar::CurrentExcelName = EMTableGrammar::CurrentExcelName + TEXT("+") + SheetBuilders[i]->GetAnalizers()[j]->GetExcelName();
			}
		}
		
		EMTableGrammar::CurrentTableName = *SheetBuilderMemoryFile->GetSheetName();

		if (false == EMTableConfig::Get().GetUAssetExportSection().IsMerge)
		{
			DataSheetPath = FString::Printf(TEXT("%s/%s"), *EMTableConfig::Get().GetUAssetExportSection().ContentsPath, *SheetBuilders[i]->GetSheetName());
			DataPackage = CreatePackage(*DataSheetPath);
			DataPackage->GetMetaData();
		}

		TSharedPtr<EMTableCore::FEMTableMemoryFile> MemFile = SheetBuilderMemoryFile->GetMemFile();
		if (false == MemFile.IsValid())
		{
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_SheetBuilder_EmptyMemory,
				TEXT("SheetBuilder중 메모리가 부족합니다."), EMTableCore::EEMSyntexException::Warning);
			continue;
		}

		MemFile->SetSeek(0, SEEK_SET);

		EMTableCore::FEMTableBinaryReader Reader(MemFile);
		UDataTable* DataTable = CreateUAsset(DataPackage, Reader, SheetBuilders[i]);
		if (nullptr == DataTable)
		{
			return false;
		}
		DataTable->AddToRoot();
		DataTables.Emplace(DataTable);
	}
	return true;
}
