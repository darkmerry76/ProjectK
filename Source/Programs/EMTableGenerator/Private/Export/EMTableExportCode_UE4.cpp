#include "EMTableExportCode_UE4.h"
#include "SheetBuilder/EMSheetBuilder.h"
#include "Config/EMTableConfig.h"
#include "Parser/EMCppParser.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "SheetBuilder/EMStructure.h"
#include "Utility/EMTableUtility.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// FExportCode
// UE4 코드를 생성하는 외부에 노출되지 않는 클래스
////////////////////////////////////////////////////////////////////////////////////////////////////
class FExportCode
{
public:	
	// 각각의 Sheet변수들을 TableRow Struct로 생성
	// 각각의 파일로 생성한다.
	static bool ExportCode_Header_UE4_Struct_Single(
		const FString& ExportFilename, int32 Index, const TSharedPtr<EMSheetBuilder>& SheetBuilder);

	// 각각의 Sheet변수들을 TableRow Struct로 생성
	// 하나의 파일로 묶어서 생성한다.
	static bool ExportCode_Header_UE4_Struct_Merge(
		const FString& ExportFilename, const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders);

	// 블루 프린트에 노출되도록 함수 라이브러리 헤더파일 생성
	static bool ExportCode_Header_UE4_BlueprintLibaray(
		const FString& ExportFilename, const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders);

	static bool ExportCode_Header_UE4_Enums(const FString& ExportFilename);

	static bool WriteStructures(const FString& ExportFilename);

private:

	static FString WriteStructure(const TSharedPtr<FEMStructure>& Structure);

	static FString WriteHeaderStruct_Head(int32 SheetIndex, const TSharedPtr<EMSheetBuilder>& SheetBuilder);
	static FString WriteHeaderStructs_Head(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders);

	// 각각의 Sheet변수들을 TableRow Struct로 생성
	static FString WriteHeaderStruct_Body(int32 SheetIndex, const TSharedPtr<EMSheetBuilder>& SheetBuilder);
	static FString WriteHeaderStructs_Body(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders);

	static FString WriteHeaderStruct_Tail(int32 SheetIndex, const TSharedPtr<EMSheetBuilder>& SheetBuilder);
	static FString WriteHeaderStructs_Tail(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders);

	static TSet<FEMStructure*> StrcutreWriteStateMap;
};

TSet<FEMStructure*> FExportCode::StrcutreWriteStateMap;

FString FExportCode::WriteStructure(const TSharedPtr<FEMStructure>& Structure)
{
	FString ResultString;

	if (true == StrcutreWriteStateMap.Contains(Structure.Get()))
	{
		return ResultString;
	}
	

	if (false == Structure.IsValid() || true == Structure->IsTableStructure())
	{
		return ResultString;
	}
	StrcutreWriteStateMap.Emplace(Structure.Get());

	for (int32 i = 0; i < Structure->GetMemberCount(); ++i)
	{
		TSharedPtr<FEMStructureMember> StructureMember = Structure->GetMember(i);

		TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(StructureMember->GetType());
		if (true == MemberStructure.IsValid())
		{
			ResultString += WriteStructure(MemberStructure);
		}
	}

	FString StructureClassName = *FEMStructure::GetClassNameUE4(Structure.Get());

	ResultString += FString::Printf(TEXT("////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));
	ResultString += FString::Printf(TEXT("// %s\r\n"), *StructureClassName);
	ResultString += FString::Printf(TEXT("////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));

	ResultString += FString::Printf(TEXT("USTRUCT(BlueprintType)\r\n"));

	ResultString += FString::Printf(TEXT("struct %s\r\n"), *StructureClassName);
	ResultString += FString::Printf(TEXT("{\r\n"));
	ResultString += FString::Printf(TEXT("\tGENERATED_USTRUCT_BODY()\r\n\r\n"));

	for (int32 i = 0; i < Structure->GetMemberCount(); ++i)
	{
		TSharedPtr<FEMStructureMember> StructureMember = Structure->GetMember(i);

		if (i != 0)
		{
			ResultString += FString::Printf(TEXT("\r\n"));
		}
		if (StructureMember->GetType() == TEXT("double"))
		{
			ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere)\r\n"));
		}
		else
		{
			ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, BlueprintReadWrite)\r\n"));
		}

		FString FinalTypeName;

		TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(StructureMember->GetType());
		if (true == MemberStructure.IsValid())
		{
			FinalTypeName = FEMStructure::GetClassNameUE4(MemberStructure.Get());
		}
		else
		{
			FinalTypeName = FEMStructure::GetTypeNameUE4(StructureMember->GetType()).ToString();
		}
		if (true == StructureMember->IsArray())
		{
			ResultString += FString::Printf(TEXT("\tTArray<%s> %s = { };\r\n"), *FinalTypeName, *StructureMember->GetName().ToString());
		}
		else
		{
			if (StructureMember->GetDefaultValue().IsEmpty() == false)
			{
				ResultString += FString::Printf(TEXT("\t%s %s = %s;\r\n"), *FinalTypeName, *StructureMember->GetName().ToString(), *StructureMember->GetDefaultValue());
			}
			else
			{
				ResultString += FString::Printf(TEXT("\t%s %s;\r\n"), *FinalTypeName, *StructureMember->GetName().ToString());
			}
		}
	}

	ResultString += FString::Printf(TEXT("};\r\n\r\n"));

	return ResultString;
}

bool FExportCode::WriteStructures(const FString& ExportFilename)
{
	FString ResultString;

	StrcutreWriteStateMap.Empty();

	ResultString += FString::Printf(TEXT("#pragma once\r\n"));
	ResultString += FString::Printf(TEXT("\r\n"));
	ResultString += FString::Printf(TEXT("#include \"CoreMinimal.h\"\r\n"));

	ResultString += FString::Printf(TEXT("#include \"%s.h\"\r\n"), *EMTableConfig::Get().GetCodeSection().EnumMergeFilename);
	
	const FString StructureFilename = EMTableUtility::GetFullPathToFilename(ExportFilename);

	const TMap<FName, TSharedPtr<FEMStructure>>& Structures = FEMStructureManager::Inst().GetStructures();
	if (0 < Structures.Num())
	{
		ResultString += FString::Printf(TEXT("#include \"%s.generated.h\"\r\n"), *StructureFilename);
		ResultString += FString::Printf(TEXT("\r\n"));
	}

	for (auto StructureIt : Structures)
	{
		ResultString += WriteStructure(StructureIt.Value);
	}
	return EMTableUtility::WriteUTF8FileW(*ExportFilename, ResultString);
}

FString FExportCode::WriteHeaderStruct_Head(int32 SheetIndex, const TSharedPtr<EMSheetBuilder>& SheetBuilder)
{
	FString ResultString;

	return ResultString;
}

FString FExportCode::WriteHeaderStruct_Body(int32 SheetIndex, const TSharedPtr<EMSheetBuilder>& SheetBuilder)
{
	FString ResultString;
	const TArray<EMTableCore::FEMVariable>& WorldVariables = SheetBuilder->GetWorldVariables();
	if (1 > WorldVariables.Num())
	{
		return ResultString;
	}

	ResultString += FString::Printf(TEXT("////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));
	ResultString += FString::Printf(TEXT("// F%sRow\r\n"), *SheetBuilder->GetSheetName());
	ResultString += FString::Printf(TEXT("////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));

	ResultString += FString::Printf(TEXT("USTRUCT(BlueprintType)\r\n"));
	if (false == SheetBuilder->GetParent().IsValid())
	{
		ResultString += FString::Printf(TEXT("struct F%sRow : public FTableRowBase\r\n"), *SheetBuilder->GetSheetName());
	}
	else
	{
		ResultString += FString::Printf(TEXT("struct F%sRow : public F%sRow\r\n"), *SheetBuilder->GetSheetName(), *SheetBuilder->GetParent().Pin()->GetSheetName());
	}
	ResultString += FString::Printf(TEXT("{\r\n"));
	ResultString += FString::Printf(TEXT("\tGENERATED_USTRUCT_BODY()\r\n\r\n"));

	int32 Count = 0;
	bool IsLastPrivate = false;

	const TArray<EMTableCore::FEMVariable>& LocalVariables = SheetBuilder->GetLocalVariables();

	for (int32 i = 0; i < LocalVariables.Num(); i++)
	{
		const EMTableCore::FEMVariabaleTypeInfo& Info = EMTableCore::GetVariableInfo(LocalVariables[i].Type);

		if (EMTableCore::EEMVariabaleType::None == LocalVariables[i].Type ||
			EMTableCore::EEMVariabaleType::Reserved == LocalVariables[i].Type)
		{
			continue;
		}
		if (INDEX_NONE == LocalVariables[i].Owner)
		{
			if (false == LocalVariables[i].IsKey)
			{
				if (EMTableCore::EEMLinkVariableType::Table == LocalVariables[i].LinkType)
				{
					if (false == IsLastPrivate)
					{
						ResultString += FString::Printf(TEXT("private:\r\n"));
						IsLastPrivate = true;
					}
					if (0 < LocalVariables[i].Description.Len())
					{
						ResultString += FString::Printf(TEXT("\t// %s\r\n"), *LocalVariables[i].Description);
					}
					ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, DisplayName = %s))\r\n"),
						*LocalVariables[i].Name);

				}
				else
				{
					if (true == IsLastPrivate)
					{
						ResultString += FString::Printf(TEXT("public:\r\n"));
						IsLastPrivate = false;
					}
					if (0 < LocalVariables[i].Description.Len())
					{
						ResultString += FString::Printf(TEXT("\t// %s\r\n"), *LocalVariables[i].Description);
					}

					if (0 < LocalVariables[i].MetaData.Len())
					{
						ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, BlueprintReadWrite, %s)\r\n"), *LocalVariables[i].MetaData);
					}
					else
					{
						if (false == Info.IsBlueprintType)
						{
							ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere)\r\n"));
						}
						else
						{
							ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, BlueprintReadWrite)\r\n"));
						}
					}
				}
			}
			else
			{
				if (0 < LocalVariables[i].Description.Len())
				{
					ResultString += FString::Printf(TEXT("\t// %s\r\n"), *LocalVariables[i].Description);
				}
				if (0 < LocalVariables[i].MetaData.Len())
				{
					if (false == Info.IsBlueprintType)
					{
						ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, %s)\r\n"), *LocalVariables[i].MetaData);
					}
					else
					{
						ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, BlueprintReadOnly, %s)\r\n"), *LocalVariables[i].MetaData);
					}
				}
				else
				{
					if (false == Info.IsBlueprintType)
					{
						ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere)\r\n"));
					}
					else
					{
						ResultString += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, BlueprintReadOnly)\r\n"));
					}
				}
			}

			// 김태균
			// Enum타잎인가?
			if (EMTableCore::EEMEnumVariableType::Type_None != LocalVariables[i].ExtraType)
			{
				FString DefaultEnumMemberName;
				const TSharedPtr<EMCppEnumNode> EnumNode = EMCppParser::Get().FindEnum(LocalVariables[i].ExtraName);
				if (false == LocalVariables[i].IsArray)
				{
					if (true == EnumNode.IsValid() && 0 < EnumNode->Members.Num() && true == EnumNode->Members[0].IsValid())
					{
						DefaultEnumMemberName = FString::Printf(TEXT(" = { %s::%s }"), *EnumNode->Name, *EnumNode->Members[0]->Name);
					}
				}
				if (true == LocalVariables[i].IsArray)
				{
					FString EnumArrayName = FString::Printf(TEXT("TArray<%s>"), *LocalVariables[i].ExtraName);

					ResultString += FString::Printf(TEXT("\t%-20s %s%s;\r\n"), *EnumArrayName, *LocalVariables[i].Name, *DefaultEnumMemberName);
				}
				else
				{
					ResultString += FString::Printf(TEXT("\t%-20s %s%s;\r\n"),
						*LocalVariables[i].ExtraName, *LocalVariables[i].Name, *DefaultEnumMemberName);
				}
			}
			// 김태균
			// 테이블 링크타잎인가?
			else if (EMTableCore::EEMLinkVariableType::Table == LocalVariables[i].LinkType)
			{
				ResultString += FString::Printf(TEXT("\t%-20s %s_Private;\r\n"), *Info.ClientTypeName, *LocalVariables[i].Name);
				ResultString += FString::Printf(TEXT("public:\r\n"));

				FString LinkTypeName;
				if (true == LocalVariables[i].IsArray)
				{
					ResultString += FString::Printf(TEXT("TArray<const struct F%sRow*>"), *LocalVariables[i].LinkName);
					ResultString += FString::Printf(TEXT("\t%-20s %s;\r\n"), *LinkTypeName, *LocalVariables[i].Name);
				}
				else
				{
					LinkTypeName = FString::Printf(TEXT("const struct F%sRow*"), *LocalVariables[i].LinkName);
					ResultString += FString::Printf(TEXT("\t%-20s %s = nullptr;\r\n"), *LinkTypeName, *LocalVariables[i].Name);
				}
				IsLastPrivate = false;
			}
			// 김태균
			// 스트링에서 변환되는 언리얼 타잎인가?
			else if (EMTableCore::EEMVariabaleType::StringToClass == LocalVariables[i].Type || EMTableCore::EEMVariabaleType::StringToObject == LocalVariables[i].Type)
			{
				if (true == LocalVariables[i].IsArray)
				{
					ResultString += FString::Printf(TEXT("\t%-20s %s;\r\n"),
						*FString::Printf(TEXT("TArray<%s>"), *LocalVariables[i].ExtraName), *LocalVariables[i].Name);
				}
				else
				{
					ResultString += FString::Printf(TEXT("\t%-20s %s%s;\r\n"),
						*LocalVariables[i].ExtraName, *LocalVariables[i].Name, *Info.InitalizeValue);
				}
			}
			// 김태균
			// 기본타잎인가?
			else
			{
				FString FinalClientType = Info.ClientTypeName;
				if (EMTableCore::EEMVariabaleType::JsonStruct == LocalVariables[i].Type)
				{
					TSharedPtr<FEMStructure> TypeStructure = FEMStructureManager::Inst().GetStructure(*LocalVariables[i].ExtraName);
					if(true == TypeStructure.IsValid())
					{
						FinalClientType = FEMStructure::GetClassNameUE4(TypeStructure.Get());
					}
				}
				if (true == LocalVariables[i].IsArray)
				{

					ResultString += FString::Printf(TEXT("\t%-20s %s;\r\n"),
						*FString::Printf(TEXT("TArray<%s>"), *FinalClientType), *LocalVariables[i].Name);
				}
				else
				{
					ResultString += FString::Printf(TEXT("\t%-20s %s%s;\r\n"),
						*FinalClientType, *LocalVariables[i].Name, *Info.InitalizeValue);
				}
			}
			ResultString += FString::Printf(TEXT("\r\n"));
		}
		Count++;
	}
	if (true == IsLastPrivate)
	{
		ResultString += FString::Printf(TEXT("public:\r\n"));
	}

	if (nullptr == SheetBuilder->GetParent())
	{
		ResultString += FString::Printf(TEXT("\tvirtual void PostLoadRowTable() { };\r\n\r\n"));
	}
	else
	{
		ResultString += FString::Printf(TEXT("////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));
		ResultString += FString::Printf(TEXT("\tinline virtual void PostLoadRowTable() override;\r\n\r\n"));
	}

	bool IsObjectStructure = false;
	const TSharedPtr<FEMStructure> ObjectRootStructure = FEMStructureManager::Inst().GetStructure(*(SheetBuilder->GetSheetName() + TEXT("::schema")));
	if (true == ObjectRootStructure.IsValid())
	{
		IsObjectStructure = !ObjectRootStructure->IsArray();
	}

	if (true == IsObjectStructure)
	{
		ResultString += FString::Printf(TEXT("\tinline static const F%sRow& Get();"), *SheetBuilder->GetSheetName());
		ResultString += FString::Printf(TEXT("\r\n"));

	}
	else
	{
		const TArray<EMTableCore::FEMVariable> KeyColumns = SheetBuilder->GetKeyColumns();

		ResultString += FString::Printf(TEXT("\tinline static const F%sRow* FindRowPtr("),

			*SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());

		if(0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%s %sKey%s"),
					KeyVariable.ExtraName.Len() > 0 ?
					*KeyVariable.ExtraName : *EMTableCore::GetVariableInfo(KeyVariable.Type).ClientTypeName,
					*KeyVariable.Name, i < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("int32 IndexKey"));
		}
		ResultString += FString::Printf(TEXT(");\r\n"));

		ResultString += FString::Printf(TEXT("\tinline static const F%sRow& FindRow("),
			*SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());

		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%s %sKey%s"),
					KeyVariable.ExtraName.Len() > 0 ?
					*KeyVariable.ExtraName : *EMTableCore::GetVariableInfo(KeyVariable.Type).ClientTypeName,
					*KeyVariable.Name, i < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("int32 IndexKey"));
		}
		ResultString += FString::Printf(TEXT(");\r\n"));


		ResultString += FString::Printf(TEXT("\tinline static FString MakeTableKeyToString("));
		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%s %sKey%s"),
					KeyVariable.ExtraName.Len() > 0 ?
					*KeyVariable.ExtraName : *EMTableCore::GetVariableInfo(KeyVariable.Type).ClientTypeName,
					*KeyVariable.Name, i < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("int32 IndexKey"));
		}
		ResultString += FString::Printf(TEXT(");\r\n"));
	}

	if(INDEX_NONE != SheetIndex)
	{
		ResultString += FString::Printf(TEXT("\r\n"));
		ResultString += FString::Printf(TEXT("\t%-20s %s = %d;\r\n"), TEXT("static const int32"), TEXT("RefTableIndex"), SheetIndex);
	}
	ResultString += FString::Printf(TEXT("};\r\n\r\n"));

	return ResultString;
}

FString FExportCode::WriteHeaderStruct_Tail(int32 SheetIndex, const TSharedPtr<EMSheetBuilder>& SheetBuilder)
{
	FString ResultString;

	const TArray<EMTableCore::FEMVariable> WorldVariables = SheetBuilder->GetWorldVariables();

	if (1 > WorldVariables.Num())
	{
		return ResultString;
	}	

	const TArray<EMTableCore::FEMVariable> KeyColumns = SheetBuilder->GetKeyColumns();

	const TArray<EMTableCore::FEMVariable>& LocalVariables = SheetBuilder->GetLocalVariables();

	ResultString += FString::Printf(TEXT("////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));
	ResultString += FString::Printf(TEXT("// %s Inline\r\n"), *SheetBuilder->GetSheetName());
	ResultString += FString::Printf(TEXT("////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));

	ResultString += FString::Printf(TEXT("inline void F%sRow::PostLoadRowTable()\r\n"), *SheetBuilder->GetSheetName());
	ResultString += FString::Printf(TEXT("{\r\n"));

	bool IsFirst = true;
	for (int32 i = 0; i < LocalVariables.Num(); i++)
	{
		if (EMTableCore::EEMLinkVariableType::Table != LocalVariables[i].LinkType)
		{
			continue;
		}
		if (false == IsFirst)
		{
			ResultString += FString::Printf(TEXT("\r\n"));
		}
		else
		{
			IsFirst = false;
		}
		if (true == LocalVariables[i].IsArray)
		{
			ResultString += FString::Printf(TEXT("\t%s.Empty();\r\n"), *LocalVariables[i].Name);
			ResultString += FString::Printf(TEXT("\tfor(int32 i=0; i < %s_Private.Num(); i++)\r\n"), *LocalVariables[i].Name);
			ResultString += FString::Printf(TEXT("\t{\r\n"));
			ResultString += FString::Printf(TEXT("\t\t%s.Emplace(F%sRow::FindRowPtr(%s_Private[i]));\r\n"),
				*LocalVariables[i].Name,
				*LocalVariables[i].LinkName,
				*LocalVariables[i].Name);

			ResultString += FString::Printf(TEXT("\t}\r\n"));
		}
		else
		{
			ResultString += FString::Printf(TEXT("\t%s = F%sRow::FindRowPtr(%s_Private);\r\n"),
				*LocalVariables[i].Name,
				*LocalVariables[i].LinkName,
				*LocalVariables[i].Name);
		}
	}

	ResultString += FString::Printf(TEXT("\tBaseScriptStruct = this->StaticStruct();\r\n"));
	ResultString += FString::Printf(TEXT("}\r\n\r\n"));

	bool IsObjectStructure = false;
	const TSharedPtr<FEMStructure> ObjectRootStructure = FEMStructureManager::Inst().GetStructure(*(SheetBuilder->GetSheetName() + TEXT("::schema")));
	if (true == ObjectRootStructure.IsValid())
	{
		IsObjectStructure = !ObjectRootStructure->IsArray();
	}
	if (true == IsObjectStructure)
	{
		ResultString += FString::Printf(TEXT("inline const F%sRow& F%sRow::Get()"), *SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());
		ResultString += FString::Printf(TEXT("\r\n"));
		ResultString += FString::Printf(TEXT("{\r\n"));
		ResultString += FString::Printf(TEXT("\tstatic const F%sRow* RowData = nullptr;\r\n"), *SheetBuilder->GetSheetName());
		ResultString += FString::Printf(TEXT("\tif(RowData == nullptr)"), *SheetBuilder->GetSheetName());
		ResultString += FString::Printf(TEXT("\r\n"));
		ResultString += FString::Printf(TEXT("\t{\r\n"));
		ResultString += FString::Printf(
			TEXT("\t\tRowData = F%s::Get().FindRowPtr<F%sRow>(0);\r\n"), *EMTableConfig::Get().GetCodeSection().TableHelperClassName, *SheetBuilder->GetSheetName());
		ResultString += FString::Printf(TEXT("\t}\r\n"));
		ResultString += FString::Printf(TEXT("\treturn *RowData;\r\n"));
		ResultString += FString::Printf(TEXT("}\r\n"));
		ResultString += FString::Printf(TEXT("\r\n"));
	}
	else
	{
		ResultString += FString::Printf(TEXT("inline FString F%sRow::MakeTableKeyToString("), *SheetBuilder->GetSheetName());
		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%s %sKey%s"),
					KeyVariable.ExtraName.Len() > 0 ?
					*KeyVariable.ExtraName : *EMTableCore::GetVariableInfo(KeyVariable.Type).ClientTypeName,
					*KeyVariable.Name, i < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("int32 IndexKey"));
		}

		ResultString += FString::Printf(TEXT(")\r\n"));

		ResultString += FString::Printf(TEXT("{\r\n"));

		for (int32 i = 0; i < KeyColumns.Num(); i++)
		{
			const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];
			if (KeyVariable.Type == EMTableCore::EEMVariabaleType::Enum)
			{
				ResultString += FString::Printf(TEXT("\tstatic const UEnum* %sKeyEnumPtr = FindObject<UEnum>(nullptr, TEXT(\"%s\"), true);\r\n"), *KeyVariable.Name, *KeyVariable.ExtraName);
				ResultString += FString::Printf(TEXT("\tcheck(nullptr != %sKeyEnumPtr);\r\n"), *KeyVariable.Name);
				ResultString += TEXT("\r\n");
			}
		}

		ResultString += FString::Printf(TEXT("\treturn F%s::Get().MakeTableIndexToString("), *EMTableConfig::Get().GetCodeSection().TableHelperClassName);
		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				if (KeyVariable.Type == EMTableCore::EEMVariabaleType::Enum)
				{
					ResultString += FString::Printf(TEXT("%sKeyEnumPtr->GetNameStringByValue((int64) %sKey)%s"),
						*KeyVariable.Name, *KeyVariable.Name, (i < KeyColumns.Num() - 1) ? TEXT(", ") : TEXT(""));
				}
				else
				{
					ResultString += FString::Printf(TEXT("%s%sKey%s"),
						(0 < KeyVariable.ExtraName.Len()) ? TEXT("(int32) ") : TEXT(""),
						*KeyVariable.Name, (i < KeyColumns.Num() - 1) ? TEXT(", ") : TEXT(""));
				}
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("IndexKey"));
		}

		ResultString += FString::Printf(TEXT(");\r\n"));
		ResultString += FString::Printf(TEXT("}\r\n\r\n"));

		ResultString += FString::Printf(TEXT("inline const F%sRow* F%sRow::FindRowPtr("),
			*SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());

		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%s %sKey%s"),
					KeyVariable.ExtraName.Len() > 0 ?
					*KeyVariable.ExtraName : *EMTableCore::GetVariableInfo(KeyVariable.Type).ClientTypeName,
					*KeyVariable.Name, i < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("int32 IndexKey"));
		}

		ResultString += FString::Printf(TEXT(")\r\n"));

		ResultString += FString::Printf(TEXT("{\r\n"));

		ResultString += FString::Printf(TEXT("\treturn F%s::Get().FindRowPtr<F%sRow>(*MakeTableKeyToString("),
			*EMTableConfig::Get().GetCodeSection().TableHelperClassName, *SheetBuilder->GetSheetName());

		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%sKey%s"),
					*KeyVariable.Name, (i < KeyColumns.Num() - 1) ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("IndexKey"));
		}

		ResultString += FString::Printf(TEXT("));\r\n"));
		ResultString += FString::Printf(TEXT("}\r\n\r\n"));

		ResultString += FString::Printf(TEXT("inline const F%sRow& F%sRow::FindRow("),
			*SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());

		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%s %sKey%s"),
					KeyVariable.ExtraName.Len() > 0 ?
					*KeyVariable.ExtraName : *EMTableCore::GetVariableInfo(KeyVariable.Type).ClientTypeName,
					*KeyVariable.Name, i < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("int32 IndexKey"));
		}
		ResultString += FString::Printf(TEXT(")\r\n"));

		ResultString += FString::Printf(TEXT("{\r\n"));
		ResultString += FString::Printf(TEXT("\treturn *FindRowPtr("),
			*EMTableConfig::Get().GetCodeSection().TableHelperClassName, *SheetBuilder->GetSheetName());

		if (0 < KeyColumns.Num())
		{
			for (int32 i = 0; i < KeyColumns.Num(); i++)
			{
				const EMTableCore::FEMVariable& KeyVariable = KeyColumns[i];

				ResultString += FString::Printf(TEXT("%sKey%s"),
					*KeyVariable.Name, (i < KeyColumns.Num() - 1) ? TEXT(", ") : TEXT(""));
			}
		}
		else
		{
			ResultString += FString::Printf(TEXT("IndexKey"));
		}
		ResultString += FString::Printf(TEXT(");\r\n"));
		ResultString += FString::Printf(TEXT("}\r\n\r\n"));
	}
	return ResultString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WriteHeaderStructs_Head
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FExportCode::WriteHeaderStructs_Head(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders)
{
	FString ResultString;
	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		ResultString += WriteHeaderStruct_Head(i, SheetBuilders[i]);
	}
	return ResultString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WriteHeaderStruct_Body
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FExportCode::WriteHeaderStructs_Body(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders)
{
	FString ResultString;
	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		ResultString += WriteHeaderStruct_Body(i, SheetBuilders[i]);
	}
	return ResultString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WriteHeaderStructs_Post
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FExportCode::WriteHeaderStructs_Tail(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders)
{
	FString ResultString;
	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		ResultString += WriteHeaderStruct_Tail(i, SheetBuilders[i]);
	}
	return ResultString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExportCode_Header_UE4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FExportCode::ExportCode_Header_UE4_Struct_Single(
	const FString& ExportFilename, int32 Index, const TSharedPtr<EMSheetBuilder>& SheetBuilder)
{
	const TArray<EMTableCore::FEMVariable>& WorldVariables = SheetBuilder->GetWorldVariables();
	if (1 > WorldVariables.Num())
	{
		return false;
	}

	if (false == SheetBuilder.IsValid() || false == SheetBuilder->GetExport())
	{
		return false;
	}
	FString ResultString;

	ResultString += FString::Printf(TEXT("#pragma once\r\n"));
	ResultString += FString::Printf(TEXT("\r\n"));
	ResultString += FString::Printf(TEXT("#include \"CoreMinimal.h\"\r\n"));
	ResultString += FString::Printf(TEXT("#include \"Runtime/Engine/Classes/Engine/DataTable.h\"\r\n"));

	const TArray<EMTableCore::FEMVariable>& LocalVariables = SheetBuilder->GetLocalVariables();

	for (int32 j = 0; j < EMTableConfig::Get().GetCodeSection().AddInclude.Num(); j++)
	{
		ResultString += FString::Printf(TEXT("#include \"%s\"\r\n"), *EMTableConfig::Get().GetCodeSection().AddInclude[j]);
	}
	if (true == SheetBuilder->GetParent().IsValid() && true == SheetBuilder->GetParent().Pin()->GetExport())
	{
		ResultString += FString::Printf(TEXT("#include \"%s.h\"\r\n"), *SheetBuilder->GetParent().Pin()->GetSheetName());
	}

	for (int32 j = 0; j < LocalVariables.Num(); j++)
	{
		if (LocalVariables[j].LinkType != EMTableCore::EEMLinkVariableType::Table)
		{
			continue;
		}
		if (false == SheetBuilder->GetParent().IsValid() ||
			SheetBuilder->GetParent() == EMSheetBuilderMgr::Get().FindSheetBuilder(LocalVariables[j].LinkName))
		{
			continue;
		}
		ResultString += FString::Printf(TEXT("#include \"%s.h\"\r\n"), *LocalVariables[j].LinkName);
	}
	ResultString += FString::Printf(TEXT("#include \"%s.generated.h\"\r\n"), *SheetBuilder->GetSheetName());
	ResultString += FString::Printf(TEXT("\r\n"));

	ResultString += WriteHeaderStruct_Body(Index, SheetBuilder);
	ResultString += WriteHeaderStruct_Tail(Index, SheetBuilder);

	if (false == EMTableUtility::WriteUTF8FileW(*ExportFilename, ResultString))
	{
		EM_LOG(EMLog, Error, TEXT("Header[\"%s\"] 저장중 쓰기에 실패했습니다!!!"), *ExportFilename);
		return false;
	}

	return true;
}

bool FExportCode::ExportCode_Header_UE4_Struct_Merge(const FString& ExportFilename, const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders)
{
	FString ResultString;

	ResultString += FString::Printf(TEXT("#pragma once\r\n"));
	ResultString += FString::Printf(TEXT("\r\n"));
	ResultString += FString::Printf(TEXT("#include \"CoreMinimal.h\"\r\n"));
	for (int32 j = 0; j < EMTableConfig::Get().GetCodeSection().AddInclude.Num(); j++)
	{
		ResultString += FString::Printf(TEXT("#include \"%s\"\r\n"), *EMTableConfig::Get().GetCodeSection().AddInclude[j]);
	}
	ResultString += FString::Printf(TEXT("#include \"Runtime/Engine/Classes/Engine/DataTable.h\"\r\n"));
	ResultString += FString::Printf(TEXT("#include \"%s.generated.h\"\r\n"), *EMTableConfig::Get().GetCodeSection().MergeFilename);
	ResultString += FString::Printf(TEXT("\r\n"));

	ResultString += WriteHeaderStructs_Head(SheetBuilders);
	ResultString += WriteHeaderStructs_Body(SheetBuilders);
	ResultString += WriteHeaderStructs_Tail(SheetBuilders);

	if (false == EMTableUtility::WriteUTF8FileW(*ExportFilename, ResultString))
	{
		EM_LOG(EMLog, Error, TEXT("Header[%s] 저장중 쓰기에 실패했습니다!!!"), *ExportFilename);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExportCode_Enum_Header_UE4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FExportCode::ExportCode_Header_UE4_Enums(const FString& ExportFilename)
{
	FString ResultString;

	ResultString += FString::Printf(TEXT("#pragma once\r\n"));
	ResultString += FString::Printf(TEXT("\r\n"));

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExportCode_Header_UE4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FExportCode::ExportCode_Header_UE4_BlueprintLibaray(
	const FString& ExportFilename, const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders)
{
	if (EMTableConfig::Get().GetCodeSection().ClassName.Len() <= 0 ||
		EMTableConfig::Get().GetCodeSection().TableHelperClassName.Len() <= 0)
	{
		return false;
	}

	FString ResultString;

	ResultString += FString::Printf(TEXT("#pragma once\r\n"));
	ResultString += FString::Printf(TEXT("\r\n"));
	ResultString += FString::Printf(TEXT("#include <Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h>\r\n"));

	for (int32 i = 0; i < EMTableConfig::Get().GetCodeSection().AddInclude.Num(); i++)
	{
		ResultString += FString::Printf(TEXT("#include \"%s\"\r\n"), *EMTableConfig::Get().GetCodeSection().AddInclude[i]);
	}
	if (EMTableConfig::Get().GetCodeSection().IsMerge == false)
	{
		for (int32 i = 0; i < SheetBuilders.Num(); i++)
		{
			if (false == SheetBuilders[i]->GetExport())
			{
				continue;
			}
			ResultString += FString::Printf(TEXT("#include \"%s.h\"\r\n"), *SheetBuilders[i]->GetSheetName());
		}
	}
	else
	{
		ResultString += FString::Printf(TEXT("#include \"%s.h\"\r\n"), *EMTableConfig::Get().GetCodeSection().MergeFilename);
	}

	ResultString += FString::Printf(TEXT("#include \"%s.generated.h\"\r\n"), *EMTableConfig::Get().GetCodeSection().ClassName);
	ResultString += FString::Printf(TEXT("\r\n"));
	ResultString += FString::Printf(TEXT("UCLASS()\r\n"));
	ResultString += FString::Printf(TEXT("class U%s : public UBlueprintFunctionLibrary\r\n"), *EMTableConfig::Get().GetCodeSection().ClassName);
	ResultString += FString::Printf(TEXT("{\r\n"));
	ResultString += FString::Printf(TEXT("\tGENERATED_BODY()\r\n"));
	ResultString += FString::Printf(TEXT("public:\r\n"));

	ResultString += FString::Printf(TEXT("\ttemplate<typename _TL>\r\n"));
	ResultString += FString::Printf(TEXT("\tstatic const _TL* GetTableEM(FName IndexName, bool& IsFind)\r\n"));
	ResultString += FString::Printf(TEXT("\t{\r\n"));
	ResultString += FString::Printf(TEXT("\t\tconst _TL* TableRow = FEMDataTableHelper::Get().FindRowPtr<_TL>(IndexName);\r\n"));
	ResultString += FString::Printf(TEXT("\t\tIsFind = (nullptr != TableRow) ? true : false;\r\n"));
	ResultString += FString::Printf(TEXT("\t\tif(false == IsFind)\r\n"));
	ResultString += FString::Printf(TEXT("\t\t{\r\n"));
	ResultString += FString::Printf(TEXT("\t\t\tstatic _TL Default;\r\n"));
	ResultString += FString::Printf(TEXT("\t\t\treturn &Default;\r\n"));
	ResultString += FString::Printf(TEXT("\t\t}\r\n"));
	ResultString += FString::Printf(TEXT("\t\treturn TableRow;\r\n"));
	ResultString += FString::Printf(TEXT("\t}\r\n"));
	ResultString += FString::Printf(TEXT("\r\n"));

	ResultString += FString::Printf(TEXT("\t////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));
	ResultString += FString::Printf(TEXT("\t// 블루프린트에 노출되는 함수 모음\r\n"));
	ResultString += FString::Printf(TEXT("\t////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"));

	ResultString += FString::Printf(TEXT("\r\n"));
	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		const TSharedPtr<EMSheetBuilder>& SheetBuilder = SheetBuilders[i];
		ensure(false != SheetBuilder.IsValid());

		const TArray<EMTableCore::FEMVariable>& LocalVariables = SheetBuilder->GetLocalVariables();

		if (1 > LocalVariables.Num())
		{
			continue;
		}

		bool IsObjectStructure = false;
		const TSharedPtr<FEMStructure> ObjectRootStructure = FEMStructureManager::Inst().GetStructure(*(SheetBuilder->GetSheetName() + TEXT("::schema")));
		if (true == ObjectRootStructure.IsValid())
		{
			IsObjectStructure = !ObjectRootStructure->IsArray();
		}
		if (true == IsObjectStructure)
		{
			ResultString += FString::Printf(TEXT("\t// %s Index가 없는 블루프린트 노출함수\r\n"), *SheetBuilder->GetSheetName());
			ResultString += FString::Printf(TEXT("\tUFUNCTION(BlueprintCallable, BlueprintPure)\r\n"));
			ResultString += FString::Printf(TEXT("\tstatic const F%sRow& GetTable%s()\r\n"), *SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());
			ResultString += FString::Printf(TEXT("\t{\r\n"));
			ResultString += FString::Printf(TEXT("\t\treturn F%sRow::Get();\r\n"),*SheetBuilder->GetSheetName());
			ResultString += FString::Printf(TEXT("\t}\r\n"));
			ResultString += FString::Printf(TEXT("\r\n"));
		}
		else
		{
			const TArray<EMTableCore::FEMVariable> KeyColumns = SheetBuilder->GetKeyColumns();

			ResultString += FString::Printf(TEXT("\t// %s FName으로 검색되는 블루프린트 노출함수\r\n"), *SheetBuilder->GetSheetName());
			ResultString += FString::Printf(TEXT("\tUFUNCTION(BlueprintCallable, BlueprintPure)\r\n"));
			ResultString += FString::Printf(TEXT("\tstatic const F%sRow& GetTable%s(FName IndexName, bool& IsFind)\r\n"),
				*SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());

			ResultString += FString::Printf(TEXT("\t{\r\n"));
			ResultString += FString::Printf(TEXT("\t\treturn *GetTableEM<F%sRow>(IndexName, IsFind);\r\n"), *SheetBuilder->GetSheetName());

			ResultString += FString::Printf(TEXT("\t}\r\n\r\n"));

			ResultString += FString::Printf(TEXT("\t// %s Key값으로 검색되는 블루프린트 노출함수\r\n"), *SheetBuilder->GetSheetName());
			ResultString += FString::Printf(TEXT("\tUFUNCTION(BlueprintCallable, BlueprintPure)\r\n"));
			ResultString += FString::Printf(TEXT("\tstatic const F%sRow& GetTable%sByKey("),
				*SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());

			if (0 < KeyColumns.Num())
			{
				for (int32 j = 0; j < KeyColumns.Num(); j++)
				{
					const EMTableCore::FEMVariable& KeyVariable = KeyColumns[j];
					if (KeyVariable.Type == EMTableCore::EEMVariabaleType::Enum &&
						KeyVariable.ExtraType != EMTableCore::EEMEnumVariableType::Uint8)
					{
						switch(KeyVariable.ExtraType)
						{
							case EMTableCore::EEMEnumVariableType::Long :
							case EMTableCore::EEMEnumVariableType::Long_Long :
							case EMTableCore::EEMEnumVariableType::Int64 :
							case EMTableCore::EEMEnumVariableType::Ulong:
							case EMTableCore::EEMEnumVariableType::Ulong_Ulong:
							case EMTableCore::EEMEnumVariableType::Uint64:
								ResultString += FString::Printf(TEXT("int64 %sEnumValueKey%s"),
									*KeyVariable.Name, j < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
								break;
							default :
								ResultString += FString::Printf(TEXT("int32 %sEnumValueKey%s"),
									*KeyVariable.Name, j < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
						}
					}
					else
					{
						ResultString += FString::Printf(TEXT("%s %sKey%s"),
							KeyVariable.ExtraName.Len() > 0 ?
							*KeyVariable.ExtraName : *EMTableCore::GetVariableInfo(KeyVariable.Type).ClientTypeName,
							*KeyVariable.Name, j < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
					}
				}
			}
			else
			{
				ResultString += FString::Printf(TEXT("int32 IndexKey"));
			}
			ResultString += FString::Printf(TEXT(", bool& IsFind)"));
			ResultString += FString::Printf(TEXT("\r\n"));

			ResultString += FString::Printf(TEXT("\t{\r\n"));

			ResultString += FString::Printf(TEXT("\t\treturn *GetTableEM<F%sRow>(*F%sRow::MakeTableKeyToString("), *SheetBuilder->GetSheetName(), *SheetBuilder->GetSheetName());

			if (0 < KeyColumns.Num())
			{
				for (int32 j = 0; j < KeyColumns.Num(); j++)
				{
					const EMTableCore::FEMVariable& KeyVariable = KeyColumns[j];

					if (KeyVariable.Type == EMTableCore::EEMVariabaleType::Enum &&
						KeyVariable.ExtraType != EMTableCore::EEMEnumVariableType::Uint8)
					{
						ResultString += FString::Printf(TEXT("%s%sEnumValueKey%s"),
							KeyVariable.ExtraName.Len() > 0 ? *FString::Printf(TEXT("(%s) "), *KeyVariable.ExtraName) : TEXT(""),
							*KeyVariable.Name, j < KeyColumns.Num() - 1 ? TEXT(", ") : TEXT(""));
					}
					else
					{
						ResultString += FString::Printf(TEXT("%sKey%s"),
							*KeyVariable.Name, (j < KeyColumns.Num() - 1) ? TEXT(", ") : TEXT(""));
					}
				}
			}
			else
			{
				ResultString += FString::Printf(TEXT("IndexKey"));
			}
			ResultString += FString::Printf(TEXT("), IsFind);"));
			ResultString += FString::Printf(TEXT("\r\n"));
			ResultString += FString::Printf(TEXT("\t}\r\n\r\n"));
		}		
	}
	ResultString += FString::Printf(TEXT("};"));
	return EMTableUtility::WriteUTF8FileW(*ExportFilename, ResultString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableExportCode_UE4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableExportCode_UE4::EMTableExportCode_UE4() : EMTableExport()
{

}

bool EMTableExportCode_UE4::MergeExport()
{
	if(false == FExportCode::ExportCode_Header_UE4_Struct_Merge(
		*EMTableConfig::Get().GetCodeSection().GetFinalMergeHeaderFilename(), EMSheetBuilderMgr::Get().GetSheetBuilders()))
	{
		return false;
	}
	return true;
}

bool EMTableExportCode_UE4::SingleExport()
{
	const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders = EMSheetBuilderMgr::Get().GetSheetBuilders();

	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		if(false == FExportCode::ExportCode_Header_UE4_Struct_Single(
			*EMTableConfig::Get().GetCodeSection().GetFinalHeaderFilename(*SheetBuilders[i]->GetSheetName()), i, SheetBuilders[i]))
		{
			return false;
		}
	}

	return true;
}

bool EMTableExportCode_UE4::DoExport()
{
	if (true == EMTableConfig::Get().GetCodeSection().IsMerge)
	{
		if(false == MergeExport())
		{
			return false;
		}
	}
	else
	{
		if (false == SingleExport())
		{
			return false;
		}
	}
	if (false == FExportCode::WriteStructures(*EMTableConfig::Get().GetCodeSection().GetFinalStructureMergeFilenam()))
	{
		return false;
	}

	if (false == FExportCode::ExportCode_Header_UE4_BlueprintLibaray(
		*EMTableConfig::Get().GetCodeSection().GetFinalBPHeaderFilename(), EMSheetBuilderMgr::Get().GetSheetBuilders()))
	{
		return false;
	}

	if (false == FExportCode::ExportCode_Header_UE4_Enums(*EMTableConfig::Get().GetCodeSection().GetFinalMergeEnumFilename()))
	{
		return false;
	}
	return true;
}