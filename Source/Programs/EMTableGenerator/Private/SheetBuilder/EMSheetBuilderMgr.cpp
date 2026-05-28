#include "EMSheetBuilderMgr.h"
#include "EMTableGenerator.h"
#include "Config/EMTableConfig.h"
#include "DataTable/EMDataTable.h"
#include "SheetBuilder/EMSheetBuilder.h"
#include "Parser/EMCppParser.h"
#include "Utility/EMTableGrammar.h"

#include "Import/EMTableImportXML.h"
#include "Import/EMTableImportLibXL.h"
#include "Import/EMTableImportJSON.h"

#include "Export/EMTableExportBinary.h"
#include "Export/EMTableExportJson.h"
#include "Export/EMTableExportUAsset.h"
#include "Export/EMTableExportCode_UE4.h"
#include "Export/EMTableExportCode_ClientEnum.h"
#include "Export/EMTableExportEnumExcel.h"

#include "Core/Public/HAL/FileManagerGeneric.h"
#include "Utility/EMTableUtility.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 제일상위 SheetBuilder 이름
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ROOOT_SHEETBUILDER_NAME	TEXT("EM_TableBase")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::EMSheetBuilderMgr
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetBuilderMgr::EMSheetBuilderMgr()
{
	RootSheet = AddSheetBuilder(ROOOT_SHEETBUILDER_NAME);
	//RootSheet = MakeShareable(new EMSheetBuilder_MemoryFile(ROOOT_SHEETBUILDER_NAME, nullptr));
	RootSheet->SetExport(false);

	RegisterImporter();
	RegisterExporter();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::~EMSheetBuilderMgr
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetBuilderMgr::~EMSheetBuilderMgr()
{
	Clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::Clear
// 모든 멤버 Clear
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMSheetBuilderMgr::Clear()
{
	RootSheet = nullptr;

	SheetBuilderMaps.Empty();
	SheetBuilders.Empty();
	SheetBuilderTransform.Clear();

	Importers.Empty();
	Exporters.Empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::RegisterImporter
// 사용가능한 Importer를 등록한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMSheetBuilderMgr::RegisterImporter()
{
	ImporterFactorys.Emplace(MakeShareable(new EMTableImportFactory<EMTableImportXML>()));
	ImporterFactorys.Emplace(MakeShareable(new EMTableImportFactory<EMTableImportLibXL>()));
	ImporterFactorys.Emplace(MakeShareable(new EMTableImportFactory<EMTableImportJSON>()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::RegisterExporter
// 사용가능한 Exporter를 등록한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMSheetBuilderMgr::RegisterExporter()
{
	if (EMTableConfig::Get().GetBinaryExportSection().IsExport == true)
	{
		Exporters.Emplace(MakeShareable(new EMTableExportBinary()));
	}
	if (EMTableConfig::Get().GetUAssetExportSection().IsExport == true)
	{
		Exporters.Emplace(MakeShareable(new EMTableExportUAsset()));
	}
	if (EMTableConfig::Get().GetJsonExportSection().IsExport == true)
	{
		Exporters.Emplace(MakeShareable(new EMTableExportJson()));
	}
	if (EMTableConfig::Get().GetCodeSection().IsExport == true)
	{
		Exporters.Emplace(MakeShareable(new EMTableExportCode_UE4()));
	}
	if (EMTableConfig::Get().GetCodeSection().IsExport == true)
	{
		Exporters.Emplace(MakeShareable(new EMTableExportCode_ClientEnum()));
	}

	Exporters.Emplace(MakeShareable(new EMTableExportEnumExcel()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::FindMutableSheetBuilder
// Sheet이름으로 EMSheetBuilder를 하나 찾아준다. mutable형태
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedPtr<EMSheetBuilder> EMSheetBuilderMgr::FindMutableSheetBuilder(const FString& SheetName) const
{
	const TSharedPtr<EMSheetBuilder>* ResultFind = SheetBuilderMaps.Find(SheetName);
	if (nullptr == ResultFind)
	{
		return nullptr;
	}
	return *ResultFind;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::FindSheetBuilder
// Sheet이름으로 EMSheetBuilder를 하나 찾아준다. const형태
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TSharedPtr<EMSheetBuilder> EMSheetBuilderMgr::FindSheetBuilder(const FString& SheetName) const
{
	return FindMutableSheetBuilder(SheetName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::IsVaildSheetName
// Sheet이름이 유효한지 검사한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::IsVaildSheetName(const FString& xlSheetName) const
{
	FString FinalSheetName = EMTableGrammar::ChangeHierarchyName(xlSheetName);

	if (false == EMTableGrammar::IsEMPrefix(xlSheetName, EMTableConfig::Get().GetGenericSection().PrefixSheetName))
	{
		return false;
	}

	EMTableGrammar::CurrentTableName = *FinalSheetName;
	if (FinalSheetName.Len() == EMTableConfig::Get().GetGenericSection().PrefixSheetName.Len())
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_SheetName_Short,
			*FString::Printf(TEXT("시트명['%s']이(가) 너무 짧습니다."), *FinalSheetName), EMTableCore::EEMSyntexException::Error);
	}
	switch (EMTableGrammar::IsValidVariableName(*FinalSheetName))
	{
	case EVariableInValidType::EVariableInValidType_Success:
		return true;
	case EVariableInValidType::EVariableInValidType_NotWord:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_SheetName_NotWord,
			*FString::Printf(TEXT("시트명['%s']이(가) 문자 형식이 아닙니다"), *FinalSheetName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_WordIncludeSpace:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_SheetName_IncludeSpaceWord,
			*FString::Printf(TEXT("시트명['%s'] 띄어쓰기(공백)를 사용할수 없습니다."), *FinalSheetName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_FirstLetterNumber:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_SheetName_FirstLetterNumber,
			*FString::Printf(TEXT("시트명['%s'] 첫글자에는 숫자를 사용할수 없습니다."), *FinalSheetName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_ReservedWords:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_SheetName_UseReservedWord,
			*FString::Printf(TEXT("이미 변수타잎으로 정의된 시트명['%s']은 사용할수 없습니다. 다른시트명을 사용해주세요"), *FinalSheetName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_EnumUsed:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UseEnum,
			*FString::Printf(TEXT("Enum명과 곁치는 시트명['%s']을 사용했습니다.(다른시트명을 사용해주세요. Enum ListExcel파일을 참조)"), *FinalSheetName), EMTableCore::EEMSyntexException::Error);
		break;
	default:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UnkownError,
			*FString::Printf(TEXT("시트명['%s']을 사용할수 없는 알수없는 ERROR입니다."), *FinalSheetName), EMTableCore::EEMSyntexException::Error);
		break;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::FindOrAddSheetBuilder
// 하나의 SheetBuilder를 찾거나 추가한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedPtr<EMSheetBuilder> EMSheetBuilderMgr::FindOrAddSheetBuilder(const FString& SheetName, TSharedPtr<EMSheetBuilder> InParent)
{
	TSharedPtr<EMSheetBuilder>* ResultFind = SheetBuilderMaps.Find(SheetName);
	if (nullptr == ResultFind)
	{
		TSharedPtr<EMSheetBuilder_MemoryFile> NewSheetBuilder = MakeShareable(new EMSheetBuilder_MemoryFile(SheetName, InParent));
		if(true == InParent.IsValid())
		{
			InParent->GetMutableChilds().Emplace(NewSheetBuilder);
		}

		SheetBuilderMaps.Emplace(SheetName, NewSheetBuilder);
		SheetBuilders.Emplace(NewSheetBuilder);
		return NewSheetBuilder;
	}
	return *ResultFind;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::CreateHierarchySheetBuilder
// SheetBuilder를 계층구조에 맞게 각각 찾거나 추가한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMSheetBuilderMgr::AddHierarchySheetBuilder(TArray<FString>& OutSheetNames)
{
	TSharedPtr<EMSheetBuilder> LocalParentSheet = RootSheet;

	for (int32 i = 0; i < OutSheetNames.Num(); i++)
	{
		LocalParentSheet = FindOrAddSheetBuilder(OutSheetNames[i], 
			(EMTableConfig::Get().GetCodeSection().StructureType == EStructureType::EStructureType_Inheritance) ? LocalParentSheet : RootSheet);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::SeparationHierarchySheetName
// Sheet이름을 계층구조에 맞게 분리한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::SeparationHierarchySheetName(const FString& SheetName, TArray<FString>& OutHierarchySheetNames)
{
	EMStringParser StringParser;

	TMap<TCHAR, TCHAR> SquadDelims;
	SquadDelims.Emplace('<', '>');

	TSharedPtr<EMStringParseNode> StringSquadNode = StringParser.ParsingSquad(SheetName, SquadDelims);

	EMStringParseNode* StringNode = StringSquadNode.Get();
	if (nullptr == StringNode)
	{
		return false;
	}

	StringParser.CollectedString(SheetName, StringSquadNode, OutHierarchySheetNames);

	for (int32 i = 0; i < OutHierarchySheetNames.Num(); i++)
	{
		if (i > 0)
		{
			OutHierarchySheetNames[i] = FString::Printf(TEXT("%s_%s"), *OutHierarchySheetNames[i - 1], *OutHierarchySheetNames[i]);
		}
		if (false == IsVaildSheetName(*OutHierarchySheetNames[i]))
		{
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::AddSheetBuilder
// SheetBuilder를 계층구조에 맞게 생성한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedPtr<EMSheetBuilder> EMSheetBuilderMgr::AddSheetBuilder(const FString& SheetName)
{
	double SheetSecondsScope = FPlatformTime::Seconds();

	EMTableGrammar::CurrentColumn = 0;
	EMTableGrammar::CurrentRow = 0;

	EMTableGrammar::CurrentTableName = *SheetName;

	TArray<FString> HierarchySheetNames;
	if (false == SeparationHierarchySheetName(SheetName, HierarchySheetNames))
	{
		return nullptr;
	}
	if (EMTableConfig::Get().GetCodeSection().StructureType == EStructureType_Merge)
	{
		HierarchySheetNames.RemoveAt(1, HierarchySheetNames.Num() - 1);
	}
	else if (EMTableConfig::Get().GetCodeSection().StructureType == EStructureType_Base)
	{
		HierarchySheetNames.RemoveAt(0, HierarchySheetNames.Num() - 1);
	}

	AddHierarchySheetBuilder(HierarchySheetNames);

	return FindMutableSheetBuilder(HierarchySheetNames[HierarchySheetNames.Num() - 1]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ParsingEnums
// c++이나 midl파일의 enum을 파싱한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ParsingEnums()
{
	TArray<FString> OutPathFiles;
	for (int32 i = 0; i < EMTableConfig::Get().GetCodeSection().EnumCppPath.Num(); i++)
	{
		TArray<FString> TempPathFiles;
		FString WildCard = EMTableUtility::GetFullPathToFilenameExt(EMTableConfig::Get().GetCodeSection().EnumCppPath[i]);
		FString EnumPath = EMTableUtility::GetFullPathToPathname(EMTableConfig::Get().GetCodeSection().EnumCppPath[i]);
		FFileManagerGeneric::Get().FindFilesRecursive(TempPathFiles, *EnumPath, *WildCard, true, false, false);

		OutPathFiles.Append(TempPathFiles);
	}

	EM_LOG(EMLog, Display, TEXT("============ Cpp[%d]에서 Enum정보 분석 ============"), OutPathFiles.Num());

	for (int32 i = 0; i < OutPathFiles.Num(); i++)
	{
		double CppSecondsScope = FPlatformTime::Seconds();
		if (true == EMCppParser::Get().LoadFile(*OutPathFiles[i]))
		{
			FString WCharBuffer = UTF8_TO_TCHAR((ANSICHAR*) EMCppParser::Get().GetMemeFile().GetFileBuffer());
			if (false == EMCppParser::Get().DoAnalize(*WCharBuffer, WCharBuffer.Len()))
			{
				EM_LOG(EMLog, Fatal, TEXT("Cpp[%s] 분석에 실패했습니다!!!!"), *EMTableUtility::GetFullPathToFilenameExt(OutPathFiles[i]));
				return false;
			}
		}
		EM_LOG(EMLog, Display, TEXT("Cpp[%s] Enum분석 완료[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(OutPathFiles[i]), FPlatformTime::Seconds() - CppSecondsScope);
	}
	EMCppParser::Get().FinalAnalize();

	return true;
}

bool EMSheetBuilderMgr::AnalizyEnumCells(TSharedPtr<EMSheet> Sheet, int32 InColumn, int32 InRow)
{
	FString EnumName = Sheet->GetCell(InColumn, InRow - 1);
	FString ClassTypeName = Sheet->GetCell(InColumn + 1, InRow - 1);
	FString Description = Sheet->GetCell(InColumn + 2, InRow - 1);

	switch (EMTableGrammar::IsValidVariableName(*EnumName))
	{
	case EVariableInValidType::EVariableInValidType_Success:
		break;
	case EVariableInValidType::EVariableInValidType_NotWord:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_NotWord,
			*FString::Printf(TEXT("Enum명['%s']이(가) 문자 형식이 아닙니다."), *EnumName), EMTableCore::EEMSyntexException::Error);
		return false;
	case EVariableInValidType::EVariableInValidType_WordIncludeSpace:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_IncludeSpaceWord,
			*FString::Printf(TEXT("Enum명['%s']에는 띄어쓰기(공백)를 할수 없습니다."), *EnumName), EMTableCore::EEMSyntexException::Error);
		return false;
	case EVariableInValidType::EVariableInValidType_FirstLetterNumber:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_FirstCharacterNumber,
			*FString::Printf(TEXT("Enum명['%s'] 첫글자에는 숫자를 사용할 수 없습니다."), *EnumName), EMTableCore::EEMSyntexException::Error);
		return false;
	case EVariableInValidType::EVariableInValidType_ReservedWords:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UseReservedWord,
			*FString::Printf(TEXT("이미 변수타잎으로 정의된 Enum['%s']명은 사용할수 없습니다. 다른Enum명을 사용해주세요"), *EnumName), EMTableCore::EEMSyntexException::Error);
		return false;
	case EVariableInValidType::EVariableInValidType_EnumUsed:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UseEnum,
			*FString::Printf(TEXT("기존Enum명과 곁치는 Enum명['%s']을 사용했습니다.(다른Enum명을 사용해주세요. Enum ListExcel파일을 참조)"), *EnumName), EMTableCore::EEMSyntexException::Error);
		return false;
	default:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UnkownError,
			*FString::Printf(TEXT("Enum명['%s']을 사용할수 없는 알수없는 ERROR입니다."), *EnumName), EMTableCore::EEMSyntexException::Error);
		return false;
	}
	const TSharedPtr<EMCppNameSpaceNode> RootNameSpace = EMCppParser::Get().GetRootNameSpace();
	TSharedPtr<EMCppEnumNode> NewEnum = MakeShareable(new EMCppEnumNode(*EnumName, RootNameSpace.Get()));

	RootNameSpace->Enums.Emplace(*EnumName, NewEnum);
	EMCppParser::Get().GetMutableEnums().Emplace(*EnumName, NewEnum);

	NewEnum->OwnerType = eEnumOnwerType::eEnumOnwerType_Excel;
	NewEnum->VariableType = EMTableCore::GetEnumVariableInfoUnrealType(*ClassTypeName);
	NewEnum->IsClass = true;
	NewEnum->Description = Description;

	if (EMTableCore::EEMEnumVariableType::Type_None == NewEnum->VariableType)
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_EnumUknownType,
			*FString::Printf(TEXT("Enum('%s')는 알수 없는 타잎을 사용했습니다.('%s')."), *EnumName, *ClassTypeName), EMTableCore::EEMSyntexException::Error);

		return false;
	}
	
	EMTableGrammar::CurrentColumn = InColumn;
	int32 ValueColumn = INDEX_NONE;
	int32 DisplayNameColumn = INDEX_NONE;
	int32 DescriptionColumn = INDEX_NONE;
	for (int32 Column = InColumn + 1; Column < Sheet->GetLastColumn(); Column++)
	{
		EMTableGrammar::CurrentColumn = Column;
		if (true == EMTableCore::FEMTableString::IsEmptyWord(*Sheet->GetCell(Column, InRow)))
		{
			break;
		}

		if (true == Sheet->GetCell(Column, InRow).Equals(TEXT("value"), ESearchCase::IgnoreCase))
		{
			ValueColumn = Column;
		}
		else if (true == Sheet->GetCell(Column, InRow).Equals(TEXT("displayname"), ESearchCase::IgnoreCase))
		{
			DisplayNameColumn = Column;
		}
		else if (true == Sheet->GetCell(Column, InRow).Equals(TEXT("설명")) ||
			true == Sheet->GetCell(Column, InRow).Equals(TEXT("desc"), ESearchCase::IgnoreCase) ||
			true == Sheet->GetCell(Column, InRow).Equals(TEXT("description"), ESearchCase::IgnoreCase))
		{
			DescriptionColumn = Column;
		}
	}

	if (INDEX_NONE == ValueColumn)
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_EnumNotValueColumn,
			*FString::Printf(TEXT("Enum['%s'] 값을 정의하는 열이 없습니다."), *EnumName), EMTableCore::EEMSyntexException::Error);
		return false;
	}

	for (int32 Row = InRow + 1; Row < Sheet->GetLastRow(); Row++)
	{
		EMTableGrammar::CurrentRow = Row;
		
		const FString& MemberName = Sheet->GetCell(InColumn, Row);
		if (true == EMTableCore::FEMTableString::IsEmptyWord(*MemberName))
		{
			break;
		}
		switch (EMTableGrammar::IsValidVariableName(*MemberName))
		{
		case EVariableInValidType::EVariableInValidType_Success:
			break;
		case EVariableInValidType::EVariableInValidType_NotWord:
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_NotWord,
				*FString::Printf(TEXT("EnumMember명['%s']이(가) 문자 형식이 아닙니다."), *MemberName), EMTableCore::EEMSyntexException::Error);
			return false;
		case EVariableInValidType::EVariableInValidType_WordIncludeSpace:
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_IncludeSpaceWord,
				*FString::Printf(TEXT("EnumMember명['%s']에는 띄어쓰기를 할수 없습니다."), *MemberName), EMTableCore::EEMSyntexException::Error);
			return false;
		case EVariableInValidType::EVariableInValidType_FirstLetterNumber:
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_FirstCharacterNumber,
				*FString::Printf(TEXT("EnumMember명['%s'] 첫글자에는 숫자를 사용할 수 없습니다."), *MemberName), EMTableCore::EEMSyntexException::Error);
			return false;
		case EVariableInValidType::EVariableInValidType_ReservedWords:
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UseReservedWord,
				*FString::Printf(TEXT("이미 변수타잎으로 정의된 EnumMember명은 사용할수 없습니다. 다른EnumMember명을 사용해주세요"), *MemberName), EMTableCore::EEMSyntexException::Error);
			return false;
		case EVariableInValidType::EVariableInValidType_EnumUsed:
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UseEnum,
				*FString::Printf(TEXT("Enum명과 곁치는 EnumMember명['%s']을 사용했습니다.(다른 EnumMember명을 사용해주세요. Enum ListExcel파일을 참조)"), *MemberName), EMTableCore::EEMSyntexException::Error);
			return false;
		default:
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UnkownError,
				*FString::Printf(TEXT("EnumMember명['%s']을 사용할수 없는 알수없는 ERROR입니다."), *MemberName), EMTableCore::EEMSyntexException::Error);
			return false;
		}

		const FString& MemberValue = Sheet->GetCell(ValueColumn, Row);
		if (false == EMTableCore::FEMTableString::IsValidType<int32>(MemberValue))
		{
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_InVaild,
				*FString::Printf(TEXT("변수 타잎('%s')과 일치하지 않는 입력 값('%s')입니다."), *MemberName, *MemberValue),
				EMTableCore::EEMSyntexException::Error);
			return false;
		}
		FString MemberDisplayName;

		if(INDEX_NONE != DisplayNameColumn)
			MemberDisplayName = Sheet->GetCell(DisplayNameColumn, Row);

		FString MemberDescription;
		if (INDEX_NONE != DescriptionColumn)
			MemberDescription = Sheet->GetCell(DescriptionColumn, Row);

		TSharedPtr<EMCppEnumMemberNode> EnumValuePtr = MakeShareable(new EMCppEnumMemberNode(*MemberName, NewEnum.Get()));

		EnumValuePtr->Value = FCString::Atoi(*MemberValue);
		EnumValuePtr->DisplayName = MemberDisplayName;
		EnumValuePtr->Description = MemberDescription;

		NewEnum->MembersMap.Emplace(EnumValuePtr->Name, EnumValuePtr);
		NewEnum->Members.Emplace(EnumValuePtr);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ParseEnumSheets
// 하나의 EnumSheet를 파싱하여 CppParser의 Enums에 추가
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ParseEnumSheet(TSharedPtr<EMSheet> Sheet)
{
	Sheet->Build();
	for (int32 Row = 1; Row < Sheet->GetLastRow(); Row++)
	{
		EMTableGrammar::CurrentRow = Row;
		for (int32 Column = 0; Column < Sheet->GetLastColumn(); Column++)
		{
			EMTableGrammar::CurrentColumn = Column;
			const FString& CellString = Sheet->GetCell(Column, Row);

			if (true == CellString.Equals(EMTableConfig::Get().GetCodeSection().EnumDelim, ESearchCase::IgnoreCase))
			{
				AnalizyEnumCells(Sheet, Column, Row);
			}
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ParseEnumSheets
// EnumSheet들을 파싱한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ParseEnumSheets(TArray<TSharedPtr<EMSheet>> Sheets)
{
	for (int32 i = 0; i < Sheets.Num(); i++)
	{
		EMTableGrammar::CurrentExcelName = Sheets[i]->GetExcelName();
		EMTableGrammar::CurrentTableName = Sheets[i]->GetSheetName();
		if (false == ParseEnumSheet(Sheets[i]))
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ParseEnumSheet
// to-do 하나의 MetaSheet 를 파싱 해서 어디에 담아둬야 할까
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ParseMetaSheet(TSharedPtr<EMSheet> Sheet)
{
	FString SheetName = Sheet->GetSheetName();
	SheetName.RemoveFromStart(EMTableConfig::Get().GetGenericSection().PrefixMetaSheet, ESearchCase::IgnoreCase);
	TSharedPtr<EMSheetBuilder>* SheetBuilderPtr = SheetBuilderMaps.Find(SheetName);

	if (SheetBuilderPtr == nullptr)
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_MetaSheetName_InVaild,
			*FString::Printf(TEXT("메타 시트 이름['%s']이(가) 잘못되었습니다."), *Sheet->GetSheetName()), EMTableCore::EEMSyntexException::Warning);
		return false;
	}
	Sheet->Build(); 

	TSharedPtr<EMSheetBuilder> SheetBuilder = *SheetBuilderPtr;
	SheetBuilder->MetaSheet = MakeShareable(new EMMetaSheet());

	TArray<FString> VariableNames;

	// 변수명 가져오기.
	for (int32 Column = 1; Column < Sheet->GetLastColumn(); Column++)
	{
		VariableNames.Add(Sheet->GetCell(Column, 0));
	}

	for (int32 Row = 1; Row < Sheet->GetLastRow(); Row++)
	{
		//메타 키 이름 가져오기.
		const FString& MetaKeyString = Sheet->GetCell(0, Row);


		for (int32 Column = 1; Column < Sheet->GetLastColumn(); Column++)
		{
			//메타 값 이름 가져오기.
			SheetBuilder->MetaSheet->AddMeta(VariableNames[Column - 1], MetaKeyString, Sheet->GetCell(Column, Row));
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ParseMetaSheets
// MetaSheets들을 파싱한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ParseMetaSheets(TArray<TSharedPtr<EMSheet>> Sheets)
{
	for (int32 i = 0; i < Sheets.Num(); i++)
	{
		ParseMetaSheet(Sheets[i]);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ImportExcelFile
// 하나의 엑셀파일을 임포트한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ImportExcelFile(const FString& ExcelFilename)
{
	if (true == EMTableConfig::Get().GetCodeSection().EnumSheetName.IsEmpty() ||
		true == EMTableConfig::Get().GetCodeSection().EnumDelim.IsEmpty())
	{
		return true;
	}

	for (int32 i = 0; i < ImporterFactorys.Num(); ++i)
	{
		TSharedPtr<EMTableImport> NewImport = ImporterFactorys[i]->DoImport(ExcelFilename);
		if (false == NewImport.IsValid())
		{			
			continue;
		}
		for (auto it = NewImport->GetMutableSheetAnalizerMaps().CreateIterator(); it; ++it)
		{
			if (true == it->Key.Equals(EMTableConfig::Get().GetCodeSection().EnumSheetName, ESearchCase::IgnoreCase))
			{
				ParseEnumSheets(it->Value);
			}
		}
		for(auto it = NewImport->GetMutableSheetAnalizerMaps().CreateIterator(); it; ++it)
		{
			if (true == it->Key.Equals(EMTableConfig::Get().GetCodeSection().EnumSheetName, ESearchCase::IgnoreCase))
			{
				continue;
			}
			else if (it->Key.StartsWith(EMTableConfig::Get().GetGenericSection().PrefixMetaSheet, ESearchCase::IgnoreCase) == true)
			{
				continue;
			}

			TSharedPtr<EMSheetBuilder> SheetBuilder = AddSheetBuilder(it->Key);
			if (false == SheetBuilder.IsValid())
			{
				continue;
			}			

			TArray<TSharedPtr<EMSheet>> BuilderAnalizers = it->Value;
			for (int32 j = 0; j < BuilderAnalizers.Num(); ++j)
			{
				EMTableGrammar::CurrentExcelName = BuilderAnalizers[j]->GetExcelName();
				EMTableGrammar::CurrentTableName = BuilderAnalizers[j]->GetSheetName();
				BuilderAnalizers[j]->Build();
				SheetBuilder->AddAnalizer(BuilderAnalizers[j]);
			}
		}

		for (auto it = NewImport->GetMutableSheetAnalizerMaps().CreateIterator(); it; ++it)
		{
			if (it->Key.StartsWith(EMTableConfig::Get().GetGenericSection().PrefixMetaSheet, ESearchCase::IgnoreCase) == true)
			{
				ParseMetaSheets(it->Value);
			}
		}

		Importers.Emplace(NewImport);

		return true;
	}
	EM_LOG(EMLog, Display, TEXT("Import[%s] 불러오기 실패!!!"), *ExcelFilename);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ImportExcelFiles
// Config.ini에 설정한 모든 엑셀파일을 임포트 한다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ImportExcelFiles()
{
	TArray<FString> OutPathFiles;
	TArray<FString> IgnorePathFiles;

	for (int32 i = 0; i < (int)EMTableConfig::Get().GetExcelFileSection().IgnoreFiles.Num(); ++i)
	{
		TArray<FString> TempPathFiles;

		FString finalPath = EMTableConfig::Get().GetExcelFileSection().Path + EMTableUtility::GetFullPathToPathname(EMTableConfig::Get().GetExcelFileSection().IgnoreFiles[i]);
		FString finalFilename = EMTableUtility::GetFullPathToFilenameExt(EMTableConfig::Get().GetExcelFileSection().IgnoreFiles[i]);

		FFileManagerGeneric::Get().FindFilesRecursive(TempPathFiles, *finalPath, *finalFilename, true, false, false);		

		for(int32 j=0; j < TempPathFiles.Num(); ++j)
		{
			TempPathFiles[j] = EMTableUtility::GetFullPathToFullPathnameOrganize(TempPathFiles[j]);
		}
		IgnorePathFiles.Append(TempPathFiles);
	}

	for (int32 i = 0; i < (int)EMTableConfig::Get().GetExcelFileSection().Files.Num(); ++i)
	{
		TArray<FString> TempPathFiles;

		FString finalPath = EMTableConfig::Get().GetExcelFileSection().Path + TEXT("/") + EMTableUtility::GetFullPathToPathname(EMTableConfig::Get().GetExcelFileSection().Files[i]);
		FString finalFilename = EMTableUtility::GetFullPathToFilenameExt(EMTableConfig::Get().GetExcelFileSection().Files[i]);

		FFileManagerGeneric::Get().FindFilesRecursive(TempPathFiles, *finalPath, *finalFilename, true, false, false);

		for (int32 j = 0; j < TempPathFiles.Num(); ++j)
		{
			TempPathFiles[j] = EMTableUtility::GetFullPathToFullPathnameOrganize(TempPathFiles[j]);
		}

		for(int32 j =0; j < TempPathFiles.Num(); ++j)
		{
			if(INDEX_NONE == IgnorePathFiles.Find(TempPathFiles[j]))
			{
				OutPathFiles.Emplace(TempPathFiles[j]);
			}
		}
	}

	EM_LOG(EMLog, Display, TEXT("============ 엑셀파일[%d] 불러오기 ============"), OutPathFiles.Num());

	FString ExcelFilePath = EMTableUtility::GetFullPathToFullPathnameOrganize(EMTableConfig::Get().GetExcelFileSection().Path) + TEXT("/");
	for (int32 i = 0; i < OutPathFiles.Num(); i++)
	{
		FString FinalOutPathFile = EMTableUtility::GetFullPathToPathname(OutPathFiles[i]);
		//if (true == ExcelFilePath.Equals(FinalOutPathFile, ESearchCase::IgnoreCase))
		{
			ImportExcelFile(OutPathFiles[i]);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ExchangeLinkType
// 변수중에 링크가 걸려있으면 링크된 Sheet의 키타입으로 변환시켜준다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::ExchangeLinkType(EMTableCore::FEMVariable& DestVariable)
{
	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		if (true == SheetBuilders[i]->GetSheetName().Equals(DestVariable.LinkName, ESearchCase::IgnoreCase))
		{
			DestVariable.Type		= SheetBuilders[i]->GetWorldVariables()[0].Type;
			DestVariable.ExtraType	= SheetBuilders[i]->GetWorldVariables()[0].ExtraType;
			DestVariable.ExtraName	= *SheetBuilders[i]->GetSheetName();

			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::ExchangeLinkTypes
// 전체 Sheet를 돌면서 EMSheetBuilderMgr::ExchangeLinkType()를 호출
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMSheetBuilderMgr::ExchangeLinkTypes()
{
	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		EMTableGrammar::CurrentTableName = *SheetBuilders[i]->GetSheetName();
		for (int32 j = 0; j < SheetBuilders[i]->GetWorldVariables().Num(); j++)
		{
			EMTableGrammar::CurrentColumn = SheetBuilders[i]->GetWorldVariables()[j].ExcelColumn;
			EMTableGrammar::CurrentRow = SheetBuilders[i]->GetWorldVariables()[j].ExcelRow;

			if (SheetBuilders[i]->GetWorldVariables()[j].LinkType == EMTableCore::EEMLinkVariableType::Table)
			{
				if(false == ExchangeLinkType(SheetBuilders[i]->GetMutableWorldVariables()[j]))
				{
					EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_TypeUnknown, *FString::Printf(TEXT("변수'%s'는 알수 없는 타잎('%s')입니다."),
						*SheetBuilders[i]->GetWorldVariables()[j].Name, *SheetBuilders[i]->GetWorldVariables()[j].LinkName), EMTableCore::EEMSyntexException::Error);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::HierarchyShortBuilders
// SheetBilder 순서를 정렬
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TArray<TSharedPtr<EMSheetBuilder>> EMSheetBuilderMgr::HierarchyShortBuilders()
{
	TArray<TSharedPtr<EMSheetBuilder>> OutSheetBiilders;

	SheetBuilderMaps.KeySort([&](const FString& A, const FString& B) { return A < B; });

	for (auto it : SheetBuilderMaps)
	{
		if (true == it.Value.IsValid() && true == it.Value->GetExport())
		{
			OutSheetBiilders.Emplace(it.Value);
		}
	}
	return OutSheetBiilders;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr::Process
// 테이블제네레이터의 전체 흐름
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMSheetBuilderMgr::Process()
{
	// 멀티키 구분자 등록
	EMTableCore::FEMRowStructVariableListener::RegisterMultiKeyDelim(EMTableConfig::Get().GetGenericSection().MultiKeyDelim);

	if (false == ParsingEnums())
	{
		return false;
	}

	EMTableSchemaMgr::Inst().Initalize();

	if (false == ImportExcelFiles())
	{
		return false;
	}
	
	SheetBuilders = HierarchyShortBuilders();
	EMTableGrammar::ResetExceptionInfo();
	double AppSecondsScope = FPlatformTime::Seconds();

	for (int32 i = 0; i < SheetBuilders.Num(); i++)
	{
		SheetBuilders[i]->BuildHead();
	}

	EMTableGrammar::ResetExceptionInfo();
	RootSheet->ReArrangeLocalToWorldVariables();

	EMTableGrammar::ResetExceptionInfo();
	ExchangeLinkTypes();
	EMTableGrammar::ResetExceptionInfo();
	if (0 == EMTableGrammar::FatalExceptionCount)
	{
		for (int32 i = 0; i < SheetBuilders.Num(); i++)
		{		
			SheetBuilders[i]->BuildBody();
		}
		for (int32 i = 0; i < SheetBuilders.Num(); i)
		{
			if (SheetBuilders[i]->GetWorldVariables().Num() <= 0)
			{
				SheetBuilders.RemoveAt(i);
			}
			else
			{
				i++;
			}
		}
	}
	EMTableGrammar::ResetExceptionInfo();

	if (0 == EMTableGrammar::FatalExceptionCount)
	{
		SheetBuilderTransform.DoTransform(SheetBuilders);
	}
	EMTableGrammar::ResetExceptionInfo();
	if(0 == EMTableGrammar::FatalExceptionCount)
	{
		for (int32 i = 0; i < Exporters.Num(); i++)
		{
			if (false == Exporters[i]->DoExport())
			{
				return false;
			}
		}
	}
	EMTableGrammar::ResetExceptionInfo();
	if(true == EMTableConfig::Get().GetGenericSection().KeyEmptyAuto)
	{
		for (int32 i = 0; i < SheetBuilders.Num(); i++)
		{
			if(0 == SheetBuilders[i]->GetKeyColumns(false).Num())
			{
				EMTableGrammar::CurrentColumn = -1;
				EMTableGrammar::CurrentRow = -1;
				EMTableGrammar::CurrentTableName = SheetBuilders[i]->GetSheetName();

				EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Key_EmptyAuto,
					*FString::Printf(TEXT("키가 정해지지 않아 자동으로 순차 인덱스로 생성합니다.")), EMTableCore::EEMSyntexException::Warning);
			}
		}
	}
	EM_LOG(EMLog, Display, TEXT("============ 전체빌드 완료[%.3f] ============"), FPlatformTime::Seconds() - AppSecondsScope);

	EMCppParser::Get().Clear();

	Clear();

	return true;
}

