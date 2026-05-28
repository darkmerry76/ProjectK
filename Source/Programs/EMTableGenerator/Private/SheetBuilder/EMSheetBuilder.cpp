#include "EMSheetBuilder.h"
#include "EMTableGenerator.h"
#include "Config/EMTableConfig.h"
#include "Utility/EMTableUtility.h"
#include "Parser/EMCppParser.h"
#include "EMTableString.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilder
////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetBuilder::EMSheetBuilder(const FString& _SheetName, TWeakPtr<EMSheetBuilder> InParent) : EMTableCore::FEMSheetBuilder(_SheetName)
	, Parent(InParent)
	, IsExport(true)
{
	SheetName = EMTableGrammar::ChangeHierarchyName(SheetName);
}

EMSheetBuilder::~EMSheetBuilder()
{

}
const TArray<int32> EMSheetBuilder::GetKeyColumnIndices(const TArray<EMTableCore::FEMVariable>& InVariables, bool IsFillOne) const
{
	TArray<int32> ResultKeyColumns;
	for (int32 Column = 0; Column < InVariables.Num(); Column++)
	{
		if (true == InVariables[Column].IsKey)
		{
			ResultKeyColumns.Emplace(Column);
		}
	}

	if (0 == ResultKeyColumns.Num() && true == IsFillOne)
	{
		ResultKeyColumns.Emplace(0);
	}

	return ResultKeyColumns;
}

const TArray<EMTableCore::FEMVariable> EMSheetBuilder::GetKeyColumns(bool IsFillOne) const
{
	TArray<int32> KeyColumnIndices = GetKeyColumnIndices(WorldVariables, IsFillOne);

	TArray<EMTableCore::FEMVariable> ResultKeyColumns;
	for (int32 Column = 0; Column < KeyColumnIndices.Num(); Column++)
	{
		if (true == WorldVariables[KeyColumnIndices[Column]].IsKey)
		{
			ResultKeyColumns.Emplace(WorldVariables[KeyColumnIndices[Column]]);
		}
	}

	return ResultKeyColumns;
}

void EMSheetBuilder::AddAnalizer(TSharedPtr<EMSheet> Analizer)
{
	Analizers.Emplace(Analizer);
}

bool EMSheetBuilder::AdjustVariableType(const FString& Cell, EMTableCore::FEMVariable& InVariable)
{
	InVariable.Type = EMTableCore::EEMVariabaleType::None;
	InVariable.ExtraType = EMTableCore::EEMEnumVariableType::Type_None;
	InVariable.LinkType = EMTableCore::EEMLinkVariableType::None;
	InVariable.LinkName = TEXT("");
	InVariable.ExtraName = TEXT("");

	FString IgnoreKeyCell = Cell;
	
	check(EMTableConfig::Get().GetGenericSection().KeyDelims.Num() > 0 && EMTableConfig::Get().GetGenericSection().KeyDelims.Num() < 3);

	if(INDEX_NONE != Cell.Find(*EMTableConfig::Get().GetGenericSection().KeyDelims[0]))
	{
		IgnoreKeyCell = Cell;
		for(int32 i=0; i < EMTableConfig::Get().GetGenericSection().KeyDelims.Num(); i++)
		{
			IgnoreKeyCell = IgnoreKeyCell.Replace(*EMTableConfig::Get().GetGenericSection().KeyDelims[i], TEXT(""));
		}

		InVariable.IsKey = true;
	}

	FString LocalCell;

	int32 MetaDataDelimIndex = INDEX_NONE;
	if(0 < EMTableConfig::Get().GetGenericSection().MetaDataDelim.Len())
	{
		MetaDataDelimIndex = IgnoreKeyCell.Find(EMTableConfig::Get().GetGenericSection().MetaDataDelim);
		if (INDEX_NONE != MetaDataDelimIndex)
		{
			LocalCell.Append(*IgnoreKeyCell, MetaDataDelimIndex);

			InVariable.MetaData = &(*IgnoreKeyCell)[MetaDataDelimIndex + 1];
		}
	}

	if (INDEX_NONE == MetaDataDelimIndex)
	{
		LocalCell = IgnoreKeyCell;
	}

	if (0 == LocalCell.Len() || INDEX_NONE != LocalCell.FindLastCharByPredicate([](TCHAR Char) { return Char == ' ' || Char == '\t'; }))
	{
		InVariable.Type = EMTableCore::EEMVariabaleType::Reserved;
		return true;
	}

	FString FinalTypeCell;
	int32 ArrayDelimIndex = INDEX_NONE;
	ArrayDelimIndex = LocalCell.Find(TEXT("["));
	if (INDEX_NONE != ArrayDelimIndex)
	{
		int32 ArrayOpen = ArrayDelimIndex;
		FinalTypeCell.Append(*LocalCell, ArrayDelimIndex);

		int32 ArrayStringFind = LocalCell.Find(TEXT("]"));

		if (INDEX_NONE == ArrayStringFind)
		{
			return false;
		}
		int32 ArrayClose = ArrayStringFind - ArrayDelimIndex;
		int32 ArrayStringSize = ArrayClose - 1;
		if (0 < ArrayStringSize)
		{
			FString ArrayIndexStr;
			ArrayIndexStr.Append(&(*LocalCell)[ArrayOpen + 1], ArrayStringSize);
			InVariable.ArrayIndex = FCString::Atoi(*ArrayIndexStr);
		}
	}
	else
	{
		FinalTypeCell = LocalCell;
	}

	InVariable.IsArray = (INDEX_NONE != ArrayDelimIndex);

	for (int32 i = 0; i < EMTableConfig::Get().GetGenericSection().StringToClasses.Num(); i++)
	{
		if (true == FinalTypeCell.Equals(EMTableConfig::Get().GetGenericSection().StringToClasses[i].Key, ESearchCase::IgnoreCase))
		{
			InVariable.Name = EMTableCore::GetVariableInfo(EMTableCore::EEMVariabaleType::StringToClass).TableTypeName;
			InVariable.Type = EMTableCore::EEMVariabaleType::StringToClass;
			InVariable.ExtraName = EMTableConfig::Get().GetGenericSection().StringToClasses[i].Value;
			return true;
		}
	}
	for (int32 i = 0; i < EMTableConfig::Get().GetGenericSection().StringToObjects.Num(); i++)
	{
		if (true == FinalTypeCell.Equals(EMTableConfig::Get().GetGenericSection().StringToObjects[i].Key, ESearchCase::IgnoreCase))
		{
			InVariable.Name = EMTableCore::GetVariableInfo(EMTableCore::EEMVariabaleType::StringToObject).TableTypeName;
			InVariable.Type = EMTableCore::EEMVariabaleType::StringToObject;
			InVariable.ExtraName = EMTableConfig::Get().GetGenericSection().StringToObjects[i].Value;
			return true;
		}
	}

	for (int32 i = 0; i < (int32)EMTableCore::EEMVariabaleType::Max; i++)
	{
		if (true == FinalTypeCell.Equals(*EMTableCore::GetVariableInfo((EMTableCore::EEMVariabaleType) i).TableTypeName, ESearchCase::IgnoreCase))
		{
			InVariable.Name = *FinalTypeCell;
			InVariable.Type = (EMTableCore::EEMVariabaleType) i;
			return true;
		}
	}

	const TSharedPtr<EMCppEnumNode> EnumPtr = EMCppParser::Get().FindEnum(FinalTypeCell);
	if (true == EnumPtr.IsValid())
	{
		InVariable.Type = EMTableCore::EEMVariabaleType::Enum;		
		InVariable.ExtraType = EnumPtr->VariableType;
		InVariable.ExtraName = *EnumPtr->Name;
		return true;
	}

	const TSharedPtr<FEMStructure> leafStructure = FEMStructureManager::Inst().GetLeafStructure(*FinalTypeCell);
	if (leafStructure.IsValid() == true)
	{
		InVariable.Type = EMTableCore::EEMVariabaleType::JsonStruct;
		InVariable.ExtraName = leafStructure->GetName().ToString().Replace(TEXT("struct#"), TEXT("")).Replace(TEXT("[]"), TEXT(""));
		return true;
	}

	const FString StructIdentification = TEXT("struct#");
	if (0 == FinalTypeCell.Find(StructIdentification))
	{
		FString StructName = FString::Printf(TEXT("EMStructSchema::%s"), &FinalTypeCell.GetCharArray().GetData()[StructIdentification.Len()]);

		const TSharedPtr<EMCppStructNode> StructNode = EMCppParser::Get().FindStruct(StructName);

		InVariable.Type = EMTableCore::EEMVariabaleType::JsonStruct;
		if (true == StructNode.IsValid())
		{
			return true;
		}
		InVariable.ExtraName = Cell.Replace(TEXT("struct#"), TEXT("")).Replace(TEXT("[]"), TEXT(""));
		return EMTableGrammar::IsTypeToStructure(Cell.Replace(TEXT("[]"), TEXT("")));
	}
	
	InVariable.Type = EMTableCore::EEMVariabaleType::Link;
	InVariable.LinkType = EMTableCore::EEMLinkVariableType::Table;
	InVariable.LinkName = *FinalTypeCell;

	return true;
}

void EMSheetBuilder::SetVariableDescription(TArray<EMTableCore::FEMVariable>& InVariables, int32 Column, const FString& Cell)
{
	EMTableGrammar::CurrentColumn = Column;

	if (InVariables.Num() <= Column)
	{
		InVariables.SetNum(Column + 1);
	}
	InVariables[Column].Description = *Cell;
}

void EMSheetBuilder::SetVariableType(TArray<EMTableCore::FEMVariable>& InVariables, int32 Column, const FString& Cell)
{
	EMTableGrammar::CurrentColumn = Column;

	if (InVariables.Num() <= Column)
	{
		InVariables.SetNum(Column + 1);
	}
	TArray<TSharedPtr<FEMStructure>> OutStructures;

	if (false == AdjustVariableType(Cell, InVariables[Column]))
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_TypeUnknown,
			FString::Printf(TEXT("알수없는 변수('%s')타잎을 사용했습니다."), *Cell), EMTableCore::EEMSyntexException::Error);
	}

	InVariables[Column].ExcelColumn = EMTableGrammar::CurrentColumn;
	InVariables[Column].ExcelRow = EMTableGrammar::CurrentRow;
}

void EMSheetBuilder::SetVariableName(TArray<EMTableCore::FEMVariable>& InVariables, int32 Column, const FString& Cell)
{
	EMTableGrammar::CurrentColumn = Column;

	if (InVariables.Num() <= Column)
	{
		InVariables.SetNum(Column + 1);
	}
	if (InVariables[Column].Type == EMTableCore::EEMVariabaleType::Reserved)
	{		
		return;
	}
	const FString FinalVariableName = FString::Printf(TEXT("%s%s%s"),
		*EMTableConfig::Get().GetCodeSection().PrefixVariableName, *EMTableGrammar::ChangeHierarchyName(Cell), *EMTableConfig::Get().GetCodeSection().SuffixVariableName);

	switch (EMTableGrammar::IsValidVariableName(*FinalVariableName))
	{
	case EVariableInValidType::EVariableInValidType_Success:
		break;
	case EVariableInValidType::EVariableInValidType_NotWord:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_NotWord,
			*FString::Printf(TEXT("변수명['%s']이 문자 형식이 아닙니다.(한글은 절대 사용금지)"), *FinalVariableName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_WordIncludeSpace:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_IncludeSpaceWord,
			*FString::Printf(TEXT("변수이름['%s']에는 띄어쓰기(공백)를 사용할수 없습니다."), *FinalVariableName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_FirstLetterNumber:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_FirstCharacterNumber,
			*FString::Printf(TEXT("변수이름['%s'] 첫글자에 숫자가 올수 없습니다."), *FinalVariableName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_ReservedWords:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UseReservedWord,
			*FString::Printf(TEXT("이미 변수타잎으로 정의된 변수명['%s']은 사용할수 없습니다. 다른변수명을 사용해주세요"), *FinalVariableName), EMTableCore::EEMSyntexException::Error);
		break;
	case EVariableInValidType::EVariableInValidType_EnumUsed:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UseEnum,
			*FString::Printf(TEXT("Enum명과 곁치는 변수명['%s']을 사용했습니다.(다른변수명을 사용해주세요. 타잎명과 변수명을 헷갈리지 마세요!!! 여기는 변수명!!!)"), *FinalVariableName), EMTableCore::EEMSyntexException::Error);
		break;
	default:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_VariableName_UnkownError,
			*FString::Printf(TEXT("변수명['%s']을 사용할수 없는 알수없는 ERROR입니다."), *FinalVariableName), EMTableCore::EEMSyntexException::Error);
		break;
	}
	InVariables[Column].Name = *EMTableUtility::RemoveSpaceWord(FinalVariableName);
}

void EMSheetBuilder::SetVariableDataStructure(const EMTableCore::FEMVariable& DestVariable, const FString& String)
{

}

void EMSheetBuilder::EnumToIntegerString(const EMTableCore::FEMVariable& DestVariable, FString& CellString, bool SpaceAllow)
{
	const TSharedPtr<EMCppEnumNode> EnumNode = EMCppParser::Get().FindEnum(*DestVariable.ExtraName);
	if (false == EnumNode.IsValid())
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_NotFoundVariableEnum,
			FString::Printf(TEXT("Enum['%s']가 존재하지 않습니다. Enum ListExcel파일 참조해주세요"), *DestVariable.ExtraName), EMTableCore::EEMSyntexException::Error);
	}
	else
	{
		const TSharedPtr<EMCppEnumMemberNode> EnumMemberNode = EnumNode->FindMember(EnumNode->Prefix + CellString);

		if (false == EnumMemberNode.IsValid())
		{
			if (true == SpaceAllow)
			{
				CellString = FString::Printf(TEXT("%lld"), 0);
			}
			else
			{
				EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_NotFoundEnumMember, FString::Printf(TEXT("Enum멤버['%s']가 존재하지 않습니다. Enum ListExcel파일 참조해주세요."),
					*DestVariable.ExtraName, *CellString), EMTableCore::EEMSyntexException::Error);
			}
		}
		else
		{
			CellString = FString::Printf(TEXT("%lld"), EnumMemberNode->Value);
		}
	}
}

void EMSheetBuilder::SetVariableData(const EMTableCore::FEMVariable& DestVariable, TArray<FString>& ArrayString, bool SpaceAllow)
{
	if (EMTableCore::EEMVariabaleType::Enum == DestVariable.Type)
	{
		for (int32 i = 0; i < ArrayString.Num(); i++)
		{
			EnumToIntegerString(DestVariable, ArrayString[i], SpaceAllow);
		}
		if (0 > (int32)DestVariable.ExtraType || (int32)EMTableCore::EEMEnumVariableType::Type_Max <= (int32)DestVariable.ExtraType)
		{
			EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_NotConvertEnumTypeVariableType,
				FString::Printf(TEXT("Enum['%s'] CppEnum 유형에서 EMVariableType으로 변환할 수 없습니다."), *DestVariable.ExtraName), EMTableCore::EEMSyntexException::Error);
		}
	}
	else if (EMTableCore::EEMVariabaleType::StringToClass == DestVariable.Type ||
		EMTableCore::EEMVariabaleType::StringToObject == DestVariable.Type)
	{
		for (int32 i = 0; i < ArrayString.Num(); i++)
		{
			if(ArrayString[i].Len() > 0)
			{
				if(0 != ArrayString[i].Find(TEXT("/"), ESearchCase::IgnoreCase))
				{
					EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_ClassPathInValid,
						FString::Printf(TEXT("클래스 경로('%s')는 '/'(가) 포함되어야 합니다."), *ArrayString[i]), EMTableCore::EEMSyntexException::Error);
				}
				FString AssetName = EMTableUtility::GetFullPathToFilename(ArrayString[i]);
				if(false == AssetName.Contains(TEXT(".")))
				{
					if(EMTableCore::EEMVariabaleType::StringToClass == DestVariable.Type)
					{
						if(INDEX_NONE != ArrayString[i].Find(TEXT("/script/"), ESearchCase::IgnoreCase))
						{
							ArrayString[i] += FString::Printf(TEXT(".%s"), *AssetName);
						}
						else
						{
							ArrayString[i] += FString::Printf(TEXT(".%s_C"), *AssetName);
						}
					}
					else if(EMTableCore::EEMVariabaleType::StringToObject == DestVariable.Type)
					{
						ArrayString[i] += FString::Printf(TEXT(".%s"), *AssetName);
					}
				}
			}
		}
	}
}


bool EMSheetBuilder::ExceptionCaseKey(const EMTableCore::FEMVariable& KeyVariable, const EMSheet* Sheet, int32 Column, int32 Row) const
{
	if(true == KeyVariable.IsArray)
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Key_ArrayInvalid,
			FString::Printf(TEXT("배열은 키('%s') 값에 사용할 수 없습니다."), *KeyVariable.Name), EMTableCore::EEMSyntexException::Error);
		return false;
	}

	// 타잎에 맞는 값인지 확인
	const FString& KeyString = Sheet->GetCell(Column, Row);
	const EMTableCore::FEMVariabaleTypeInfo& Info = EMTableCore::GetVariableInfo(KeyVariable.Type);
	switch (EMTableGrammar::IsValidKey(KeyVariable.Type, KeyString))
	{
	case EKeyInValidType::EKeyInValidType_Empty:
		
		check(Row < Sheet->GetRow().Num());
		{
			bool IsLiveRow = false;
			for (int32 i = 0; i < Sheet->GetRow()[Row]->Columns.Num(); i++)
			{
				const FString ColumnString = Sheet->GetRow()[Row]->Columns[i];

				if (0 != ColumnString.Len() && true != EMTableCore::FEMTableString::IsEmptyWord(ColumnString))
				{
					IsLiveRow = true;
				}
			}
			if(true == IsLiveRow)
			{
				EMTableGrammar::CurrentColumn = Column;
				EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_KeyEmptyWordString,
					TEXT("키 값이 비어 있으므로 현재 위치에서 끝납니다"), EMTableCore::EEMSyntexException::Error);
			}
		}
		return false;
	case EKeyInValidType::EKeyInValidType_TypeNotMach:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_KeyInvalidType, FString::Printf(TEXT("키('%s') 타잎에 맞지 않는 값('%s')입니다."),
			*KeyString, *Info.TableTypeName), EMTableCore::EEMSyntexException::Error);
		return false;
	case EKeyInValidType::EKeyInValidType_NotSupport:
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_NotSupportType, FString::Printf(TEXT("키로 사용할 수 없는 타잎('%s')입니다."),
			*Info.TableTypeName), EMTableCore::EEMSyntexException::Error);
		return false;

	}
	return true;
}

void EMSheetBuilder::SetVariableDatas(const EMSheet* Sheet, int32 Row)
{
	if (WorldVariables.Num() <= 0)
	{
		return;
	}

	TArray<int32> KeyColumns;
	for (int32 i = 0; i < WorldVariables.Num(); i++)
	{
		if (true == WorldVariables[i].IsKey)
		{
			KeyColumns.Emplace(i);
		}
	}
	if (0 == KeyColumns.Num())
	{
		int32 emptyCount = 0;
		for (int32 i = 0; i < Sheet->GetLastColumn(); i++)
		{
			if (Sheet->GetCell(i, Row).IsEmpty() == true)
			{
				++emptyCount;
			}
		}
		
		if (emptyCount == Sheet->GetLastColumn())
		{
			return;
		}
	}
	for(int32 i=0; i < KeyColumns.Num(); i++)
	{
		int32 KeyColumn = Sheet->GetRow()[2]->FindColumn(WorldVariables[KeyColumns[i]].Name);
		if(INDEX_NONE == KeyColumn || false == ExceptionCaseKey(WorldVariables[KeyColumns[i]], Sheet, KeyColumn, Row))
		{
			return;		
		}
		if (Sheet->GetCell(KeyColumn, Row).IsEmpty() == true)
		{
			return;
		}
	}
	
	for(int32 Column=0; Column < GetWorldVariables().Num(); ++Column)
	{				
		int32 SheetColumn = Sheet->GetRow()[2]->FindColumn(WorldVariables[Column].Name);

		if (WorldVariables[Column].Type == EMTableCore::EEMVariabaleType::JsonStruct)
		{
			SetVariableDataStructure(WorldVariables[Column], Sheet->GetCell(SheetColumn, Row));
		}
		else
		{
			TArray<FString> ArrayString;
			EMTableGrammar::CurrentColumn = SheetColumn;

			if (INDEX_NONE == SheetColumn)
			{
				EMTableCore::FEMTableString::StringToArrayString(TEXT(""), EMTableConfig::Get().GetGenericSection().ArrayDelims, ArrayString);
			}
			else {
				FString SheetString = Sheet->GetCell(SheetColumn, Row);

				if (WorldVariables[Column].IsArray == false)
				{
					ArrayString.Emplace(Sheet->GetCell(SheetColumn, Row));
				}
				else
				{
					if (WorldVariables[Column].Type == EMTableCore::EEMVariabaleType::String && WorldVariables[Column].IsArray == true)
					{
						TArray<FString> stringDelims;
						stringDelims.Emplace(TEXT(",|"));
						EMTableCore::FEMTableString::StringToArrayString(Sheet->GetCell(SheetColumn, Row),
							stringDelims, ArrayString);
					}
					else
					{
						EMTableCore::FEMTableString::StringToArrayString(Sheet->GetCell(SheetColumn, Row),
							EMTableConfig::Get().GetGenericSection().ArrayDelims, ArrayString);
					}
				}

				if (WorldVariables[Column].IsArray == true && EMTableCore::FEMTableString::IsEmptyWord(SheetString) == true)
				{
					ArrayString.Empty();
				}
			}

			const EMTableCore::FEMVariabaleTypeInfo& VariableInfo = EMTableCore::GetVariableInfo(WorldVariables[Column].Type);
			if (true == VariableInfo.IsStruct)
			{
				/*FString MergeStructString = TEXT("\0");
				for (int32 i = 0; i < ArrayString.Num(); i++)
				{
					MergeStructString += ArrayString[i];
					if(i < ArrayString.Num() - 1) MergeStructString += TEXT(",");
				}
				ArrayString.Empty();
				ArrayString.Emplace(MergeStructString);*/
			}

			SetVariableData(WorldVariables[Column], ArrayString, INDEX_NONE == SheetColumn ? true : false);
		}
	}

	MaxRowCount++;
}

TArray<EMTableCore::FEMVariable> EMSheetBuilder::GetOptimizeVariables(const TArray<EMTableCore::FEMVariable>& InVariables) const
{
	TArray<EMTableCore::FEMVariable> ResultVariables;
	for (int32 i = 0; i < InVariables.Num(); i++)
	{
		if (EMTableCore::EEMVariabaleType::None == InVariables[i].Type ||
			EMTableCore::EEMVariabaleType::Reserved == InVariables[i].Type)
		{
			continue;
		}
		ResultVariables.Emplace(InVariables[i]);
	}
	return ResultVariables;
}

TArray<EMTableCore::FEMVariable> EMSheetBuilder::GetReArrangeArrayVariables(const TArray<EMTableCore::FEMVariable>& InVariables) const
{
	TArray<EMTableCore::FEMVariable> ResultVariables = InVariables;
	for (int32 i = 0; i < ResultVariables.Num(); i++)
	{
		if (false == ResultVariables[i].IsArray || INDEX_NONE != ResultVariables[i].Owner)
		{
			continue;
		}

		for (int32 j = i + 1; j < ResultVariables.Num(); j++)
		{
			if(false == ResultVariables[j].IsArray || INDEX_NONE != ResultVariables[j].Owner)
			{
				continue;
			}
			if (true == ResultVariables[j].Name.Equals(ResultVariables[i].Name))
			{
				ResultVariables[j].Owner = i;
				if (ResultVariables[i].MaxArrayCount < ResultVariables[j].ArrayIndex)
				{
					ResultVariables[i].MaxArrayCount = ResultVariables[j].ArrayIndex;
				}
			}
		}
		if (INDEX_NONE !=  ResultVariables[i].MaxArrayCount)
		{
			ResultVariables[i].MaxArrayCount = ResultVariables[i].MaxArrayCount + 1;
		}
	}
	for (int32 i = 0; i < ResultVariables.Num(); i++)
	{
		if (INDEX_NONE != ResultVariables[i].Owner)
		{
			ResultVariables[i].MaxArrayCount = ResultVariables[ResultVariables[i].Owner].MaxArrayCount;
		}
	}
	return ResultVariables;
}

bool EMSheetBuilder::MergeVariables(TArray<EMTableCore::FEMVariable>& DestVariables, const TArray<EMTableCore::FEMVariable>& SourceVariables)
{
	if (0 == DestVariables.Num())
	{
		for (int32 i = 0; i < SourceVariables.Num(); i++)
		{
			EMTableGrammar::CurrentColumn = SourceVariables[i].ExcelColumn;
			AddVariable(DestVariables, SourceVariables[i]);
		}
	}
	else
	{
		for (int32 i = 0; i < SourceVariables.Num(); i++)
		{
			EMTableGrammar::CurrentColumn = SourceVariables[i].ExcelColumn;

			const EMTableCore::FEMVariable* Result = FindVariable(DestVariables, SourceVariables[i].Name);
			if (nullptr == Result)
			{
				AddVariable(DestVariables, SourceVariables[i]);
			}
			else if (*Result != SourceVariables[i])
			{
				EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_HasTheSameStructure,
					FString::Printf(TEXT("['%s']는 구조는 같지만 순서나 타잎이 다릅니다."), *SourceVariables[i].Name), EMTableCore::EEMSyntexException::Error);
			}
		}
	}
	return true;
}

bool EMSheetBuilder::MergeLocalVariables(const TArray<EMTableCore::FEMVariable>& SourceVariables)
{
	return MergeVariables(LocalVariables, SourceVariables);
}

bool EMSheetBuilder::MergeWorldVariables(const TArray<EMTableCore::FEMVariable>& SourceVariables)
{
	return MergeVariables(WorldVariables, SourceVariables);
}

EMSheetBuilder* EMSheetBuilder::DifferenceMergeVariables(TArray<EMTableCore::FEMVariable>& SourceVariables)
{
	if (false == GetParent().IsValid())
	{
		return nullptr;
	}
	MergeLocalVariables(SourceVariables);
	return GetParent().Pin()->DifferenceMergeVariables(SourceVariables);
}

void EMSheetBuilder::BuildHead()
{
	EMTableGrammar::ResetExceptionInfo();
	EMTableGrammar::CurrentTableName = GetSheetName();
	EMTableGrammar::CurrentRow = 3;

	for (int32 i = 0; i < Analizers.Num(); ++i)
	{
		TSharedPtr<EMSheet> Sheet = Analizers[i];
		check(true == Sheet.IsValid());

		if (Sheet->GetRow().Num() < 3)
		{
			continue;
		}

		EMTableGrammar::CurrentExcelName = Sheet->GetExcelName();
		Sheet->GetRow()[2]->CreateColumnMaps();

		TArray<EMTableCore::FEMVariable> TempVariables;

		for (int32 Column = 0; Column < Sheet->GetLastColumn(); ++Column)
		{
			EMTableGrammar::CurrentColumn = Column;
			const FString& DescriptionCell = Sheet->GetCell(Column, 0);
			const FString& TypCell = Sheet->GetCell(Column, 1);
			const FString& NameCell = Sheet->GetCell(Column, 2);

			EMTableGrammar::CurrentRow = 0;
			SetVariableDescription(TempVariables, Column, DescriptionCell);
			EMTableGrammar::CurrentRow = 1;
			SetVariableType(TempVariables, Column, TypCell);
			EMTableGrammar::CurrentRow = 2;
			SetVariableName(TempVariables, Column, NameCell);

			//메타 시트가 있을 경우 메타 데이터 설정.
			if(MetaSheet != nullptr)
			{
				TempVariables[Column].MetaData = MetaSheet->GetMetaString(TempVariables[Column].Name, TempVariables[Column].MetaData);
			}
		}
		DifferenceMergeVariables(TempVariables);
	}
	LocalVariables = GetOptimizeVariables(LocalVariables);
	LocalVariables = GetReArrangeArrayVariables(LocalVariables);
}

void EMSheetBuilder::ReArrangeLocalToWorldVariables()
{
	TMap<FString, int32> OverlapCounts;

	if(true == GetParent().IsValid())
	{
		WorldVariables = GetParent().Pin()->GetWorldVariables();
	}
	else 
	{
		for (int32 i = 0; i < Childs.Num(); i++)
		{
			check(true == Childs[i].IsValid());
			Childs[i].Pin()->ReArrangeLocalToWorldVariables();
		}
		return;
	}

	int32 EqualCount = Childs.Num();

	for (int32 i = 0; i < Childs.Num(); i++)
	{
		check(true == Childs[i].IsValid());
		for (int32 j = 0; j < Childs[i].Pin()->LocalVariables.Num(); ++j)
		{
			int32* Result = OverlapCounts.Find(Childs[i].Pin()->LocalVariables[j].Name);

			if(nullptr != Result) (*Result) += 1;
			else OverlapCounts.Emplace(Childs[i].Pin()->LocalVariables[j].Name, 1);
		}
	}

	RemoveNoneOverlapsLocalVariables(OverlapCounts, EqualCount);

	WorldVariables += LocalVariables;

	RemoveChildOverlapsLocalVariables(OverlapCounts, EqualCount);

	for (int32 i = 0; i < Childs.Num(); i++)
	{
		check(true == Childs[i].IsValid());
		Childs[i].Pin()->ReArrangeLocalToWorldVariables();
	}

	WorldVariables = GetReArrangeArrayVariables(GetOptimizeVariables(WorldVariables));
}

void EMSheetBuilder::RemoveNoneOverlapsLocalVariables(const TMap<FString, int32>& OverlapCounts, int32 EqualCount)
{
	for (int32 i = 0; i < LocalVariables.Num(); i)
	{
		const int32* Result = OverlapCounts.Find(LocalVariables[i].Name);
		if (nullptr != Result && *Result != EqualCount)
		{
			LocalVariables.RemoveAt(i);
		}
		else {
			i++;
		}
	}
}

void EMSheetBuilder::RemoveChildOverlapsLocalVariables(const TMap<FString, int32>& OverlapCounts, int32 EqualCount)
{
	for(int32 i=0; i < Childs.Num(); i++)
	{
		check(true == Childs[i].IsValid());
		for (int32 j = 0; j < Childs[i].Pin()->LocalVariables.Num(); j)
		{
			const int32* Result = OverlapCounts.Find(Childs[i].Pin()->LocalVariables[j].Name);
			check(nullptr != Result);

			if (*Result == EqualCount)
			{
				Childs[i].Pin()->LocalVariables.RemoveAt(j);
			}
			else {
				j++;
			}
			Childs[i].Pin()->RemoveChildOverlapsLocalVariables(OverlapCounts, EqualCount);
		}
	}
}

void EMSheetBuilder::BuildBody()
{
	EMTableGrammar::ResetExceptionInfo();
	EMTableGrammar::CurrentTableName = GetSheetName();

	for(int32 i=0; i < Analizers.Num(); i++)
	{
		TSharedPtr<EMSheet> Sheet = Analizers[i];
		if (false == Sheet.IsValid() || 1 > Sheet->GetLastColumn())
		{
			continue;
		}
		if (Sheet->GetRow().Num() < 4)
		{
			continue;
		}

		EMTableGrammar::CurrentExcelName = Sheet->GetExcelName();

		for (int32 Row = 3; Row < Sheet->GetLastRow(); Row++)
		{
			EMTableGrammar::CurrentRow = Row;

			SetVariableDatas(Sheet.Get(), Row);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilder_MemoryFile
////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetBuilder_MemoryFile::EMSheetBuilder_MemoryFile(const FString& _SheetName, TWeakPtr<EMSheetBuilder> InParent) : EMSheetBuilder(_SheetName, InParent)
{
	MemFile = MakeShareable(new EMTableCore::FEMTableMemoryFile(500000));
}

void EMSheetBuilder_MemoryFile::SetVariableData(const EMTableCore::FEMVariable& DestVariable, TArray<FString>& ArrayString, bool SpaceAllow)
{
	EMSheetBuilder::SetVariableData(DestVariable, ArrayString, SpaceAllow);

	if (EMTableCore::EEMVariabaleType::Enum == DestVariable.Type)
	{
		if (0 <= (int32)DestVariable.ExtraType && (int32)EMTableCore::EEMEnumVariableType::Type_Max > (int32)DestVariable.ExtraType)
		{
			EMTableCore::FEMVariable EnumTypeVariable = DestVariable;
			EnumTypeVariable.Type = (EMTableCore::EEMVariabaleType) EMTableCore::GetEnumVariableInfo(DestVariable.ExtraType).ToTableType;

			SerializeValueFromString(*MemFile, EnumTypeVariable, ArrayString);
		}		
	}
	else
	{
		SerializeValueFromString(*MemFile, DestVariable, ArrayString);
	}
}

void EMSheetBuilder_MemoryFile::SetVariableDataStructure(const EMTableCore::FEMVariable& DestVariable, const FString& String)
{
	FString FinalJsonString = FString(TEXT("{\r\n\"EMroot\":")) + String + TEXT("}");
	EMSheetBuilder::SetVariableDataStructure(DestVariable, FinalJsonString);

	const TSharedPtr<FEMStructure> TypeStructure = FEMStructureManager::Inst().GetStructure(*DestVariable.ExtraName);

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(*FinalJsonString);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	bool Result = FJsonSerializer::Deserialize(Reader, JsonObject);
	ensure(true == Result);

	TSharedPtr<FJsonValue> RootValue = JsonObject->TryGetField(TEXT("EMroot"));
	if (false == DestVariable.IsArray)
	{
/*		if(RootValue->Type != EJson::Object)
		{
			ensure(0);
		}*/
		SetVariableDataStructureRecursive(*MemFile, TypeStructure, RootValue, FEMStructure::GetClassNameUE4(TypeStructure.Get()));
	}
	else
	{
		if (RootValue->Type != EJson::Array)
		{
			ensure(0);
		}
		const TArray<TSharedPtr<FJsonValue>>& StructArray = RootValue->AsArray();
		MemFile->Serialize((int16) StructArray.Num());

		for (auto Value : StructArray)
		{
			SetVariableDataStructureRecursive(*MemFile, TypeStructure, Value, FEMStructure::GetClassNameUE4(TypeStructure.Get()));
		}
	}
	
	return;
}

void EMSheetBuilder_MemoryFile::SetVariableDataStructureRecursive(EMTableCore::FEMTableFileInterface& File, 
	const TSharedPtr<FEMStructure> TypeStructure, TSharedPtr<FJsonValue> Value, FString StructName)
{
	TSharedPtr<FJsonObject> JsonObject = true == Value.IsValid() ? Value->AsObject() : nullptr;

	for (int32 i = 0; i < TypeStructure->GetMemberCount(); ++i)
	{
		TSharedPtr<FEMStructureMember> StructureMember = TypeStructure->GetMember(i);
		if (false == StructureMember.IsValid())
		{
			continue;
		}
		EMTableCore::FEMVariable TempVariable;
		TempVariable.Name = StructureMember->GetName().ToString();

		TSharedPtr<FJsonValue> MemberValue = true == JsonObject.IsValid() ? JsonObject->TryGetField(StructureMember->GetName().ToString()) : nullptr;

		const TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(*StructureMember->GetType().ToString());
		if (false == MemberValue.IsValid() && true == MemberStructure.IsValid())
		{
			SetVariableDataStructureRecursive(File, MemberStructure, nullptr, StructName + TEXT(".") + FEMStructure::GetClassNameUE4(MemberStructure.Get()));
			continue;
		}
		else if (false == MemberValue.IsValid())
		{
			TempVariable.Type = StructureMember->GetTableVariableType();
			SerializeValueFromString(File, TempVariable, StructureMember->GetDefaultValue(), StructName);
			continue;
		}

		if(true == MemberStructure.IsValid())
		{
			if (MemberValue->Type == EJson::Array)
			{
				const TArray<TSharedPtr<FJsonValue>>& StructArray = MemberValue->AsArray();
				MemFile->Serialize((int16) StructArray.Num());

				for (auto ArrayIt : StructArray)
				{
					SetVariableDataStructureRecursive(File, MemberStructure, ArrayIt, StructName + TEXT(".") + FEMStructure::GetClassNameUE4(MemberStructure.Get()));
				}
			}
			else if (MemberValue->Type == EJson::Object)
			{
				SetVariableDataStructureRecursive(File, MemberStructure, MemberValue, StructName + TEXT(".") + FEMStructure::GetClassNameUE4(MemberStructure.Get()));
			}
		}
		else
		{
			if (MemberValue->Type == EJson::String ||
				MemberValue->Type == EJson::Number ||
				MemberValue->Type == EJson::Boolean)
			{
				FString MemberString = MemberValue->AsString();
				if (StructureMember->GetTableVariableType() == EMTableCore::EEMVariabaleType::Enum)
				{
					TempVariable.ExtraName = *StructureMember->GetType().ToString();
					TempVariable.Type = EMTableCore::EEMVariabaleType::Int8;

					const TSharedPtr<EMCppEnumNode> EnumNode = EMCppParser::Get().FindEnum(*TempVariable.ExtraName);
					if (true == EnumNode.IsValid())
					{
						TempVariable.ExtraType = EnumNode->VariableType;
						TempVariable.Type = GetEnumVariableToVariableType(TempVariable.ExtraType);						
					}
					EnumToIntegerString(TempVariable, MemberString);
				}
				else
				{
					TempVariable.Type = StructureMember->GetTableVariableType();
				}

				SerializeValueFromString(File, TempVariable, MemberString, StructName);
			}
			else if(MemberValue->Type == EJson::Object)
			{
				const TSharedPtr<FJsonObject>& MemberObject = MemberValue->AsObject();
				ensure(MemberObject->Values.Num() != 0);
				TempVariable.Type = EMTableCore::EEMVariabaleType::String;
				SerializeValueFromString(File, TempVariable, TEXT(""), StructName);
			}
			else if (MemberValue->Type == EJson::Array)
			{
				const TArray<TSharedPtr<FJsonValue>>& ArrayValues = MemberValue->AsArray();

				if(EMTableCore::EEMVariabaleType::String == StructureMember->GetTableVariableType())
				{
					TempVariable.IsArray = true;
					TempVariable.Type = EMTableCore::EEMVariabaleType::String;

					TArray<FString> ArrayStrings;

					for(int32 ArrayIndex = 0; ArrayIndex < ArrayValues.Num(); ++ArrayIndex)
					{
						ArrayStrings.Emplace(ArrayValues[ArrayIndex]->AsString());
					}
					SerializeValueFromString(File, TempVariable, ArrayStrings);
				}
				else ensure(0);
			}
		}
	}
}

int64 EMSheetBuilder_MemoryFile::GetDataLength() const
{
	if (false == MemFile.IsValid())
	{
		return 0;
	}
	return MemFile->GetFileLength();
}

uint8* EMSheetBuilder_MemoryFile::GetDataBuffer() const
{
	if (false == MemFile.IsValid())
	{
		return nullptr;
	}
	return MemFile->GetFileBuffer();
}

void EMSheetBuilder_MemoryFile::BuildHead()
{
	EMSheetBuilder::BuildHead();
}

void EMSheetBuilder_MemoryFile::BuildBody()
{
	if(0 < WorldVariables.Num())
	{
		TArray<int32> KeyIndices = GetKeyColumnIndices(WorldVariables, false == EMTableConfig::Get().GetGenericSection().KeyEmptyAuto);

		if(0 < KeyIndices.Num())
		{
			WorldVariables[KeyIndices[0]].IsKey = true;
			for (int32 i = 0; i < KeyIndices.Num(); i++)
			{
				if (WorldVariables[KeyIndices[i]].Type == EMTableCore::EEMVariabaleType::String)
				{
					WorldVariables[KeyIndices[i]].Type = EMTableCore::EEMVariabaleType::Name;
				
					for (int32 j = 0; j < LocalVariables.Num(); j++)
					{
						if (LocalVariables[j].Name == WorldVariables[KeyIndices[i]].Name)
						{
							LocalVariables[j].Type = EMTableCore::EEMVariabaleType::Name;
							LocalVariables[j].IsKey = true;
						}
					}
				}
			}
		}
	}
	SerializeVariables(*MemFile, WorldVariables);

	EMSheetBuilder::BuildBody();

	MemFile->Compact();
}

bool EMSheetBuilder_MemoryFile::SerializeVariables(EMTableCore::FEMTableFileInterface& File, const TArray<EMTableCore::FEMVariable>& InVariables)
{
	for (int32 i = 0; i < InVariables.Num(); i++)
	{
		File.Serialize(InVariables[i]);
	}
	return true;
}

bool EMSheetBuilder_MemoryFile::SerializeValueFromString(EMTableCore::FEMTableFileInterface& File, const EMTableCore::FEMVariable& Variable, const FString& VariableString, FString StructName)
{
	switch (Variable.Type)
	{
	case EMTableCore::EEMVariabaleType::Boolean:
		File.Serialize(EMSheetBuilder::StringToType<bool>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Int8:
		File.Serialize(EMSheetBuilder::StringToType<int8>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Uint8:
		File.Serialize(EMSheetBuilder::StringToType<uint8>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Int16:
		File.Serialize(EMSheetBuilder::StringToType<int16>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Uint16:
		File.Serialize(EMSheetBuilder::StringToType<uint16>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Int32:
		File.Serialize(EMSheetBuilder::StringToType<int32>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Uint32:
		File.Serialize(EMSheetBuilder::StringToType<uint32>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Int64:
		File.Serialize(EMSheetBuilder::StringToType<int64>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Uint64:
		File.Serialize(EMSheetBuilder::StringToType<uint64>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Float:
		File.Serialize(EMSheetBuilder::StringToType<float>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Double:
		File.Serialize(EMSheetBuilder::StringToType<double>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Name:
		File.Serialize(EMTableCore::FEMStringA(TCHAR_TO_UTF8(*VariableString))); break;
	case EMTableCore::EEMVariabaleType::PdaKey:
		File.Serialize(EMTableCore::FEMStringA(TCHAR_TO_UTF8(*VariableString))); break;
	case EMTableCore::EEMVariabaleType::Color:
		File.Serialize(EMSheetBuilder::StringToType<FColor>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Vector2:
		File.Serialize(EMSheetBuilder::StringToType<FVector2D>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::Vector3:
		File.Serialize(EMSheetBuilder::StringToType<FVector>(Variable, VariableString)); break;
	case EMTableCore::EEMVariabaleType::String:
		File.Serialize(EMTableCore::FEMStringA(TCHAR_TO_UTF8(*VariableString))); break;
	case EMTableCore::EEMVariabaleType::StringToClass:
		File.Serialize(EMTableCore::FEMStringA(TCHAR_TO_UTF8(*VariableString))); break;
	case EMTableCore::EEMVariabaleType::StringToObject:
		File.Serialize(EMTableCore::FEMStringA(TCHAR_TO_UTF8(*VariableString))); break;

	default:ensure(false);return false;
	}
	return true;
}

bool EMSheetBuilder_MemoryFile::SerializeValueFromString(EMTableCore::FEMTableFileInterface& File, const EMTableCore::FEMVariable& Variable, const TArray<FString>& VariableStrings)
{
	const EMTableCore::FEMVariabaleTypeInfo& Info = EMTableCore::GetVariableInfo(Variable.Type);

	EMTableGrammar::CurrentColumn = Variable.ExcelColumn;
	
	if (Variable.IsArray == true && Variable.ArrayIndex == -1)
	{
		File.Serialize(static_cast<int16>(VariableStrings.Num()));
	}
	else
	{
		if (VariableStrings.Num() != 1)
		{
			return false;
		}
	}

	for (int32 i = 0; i < VariableStrings.Num(); i++)
	{
		if(false == SerializeValueFromString(File, Variable, VariableStrings[i], TEXT("")))
		{
			return false;
		}
	}
	return true;
}
