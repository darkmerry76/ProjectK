#include "EMTableGrammar.h"
#include "EMTableUtility.h"
#include "Config/EMTableConfig.h"
#include "Parser/EMCppParser.h"
#include "Programs/EMTableGenerator/Private/Config/EMTableConfig.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exception 출력에 필요한 정보
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32	EMTableGrammar::CurrentRow = 0;
int32	EMTableGrammar::CurrentColumn = 0;
FString	EMTableGrammar::CurrentExcelName;
FString	EMTableGrammar::CurrentTableName;
int32	EMTableGrammar::FatalExceptionCount = 0;
int32	EMTableGrammar::WarningExceptionCount = 0;

void EMTableGrammar::ResetExceptionInfo()
{
	EMTableGrammar::CurrentRow = 0;
	EMTableGrammar::CurrentColumn = 0;
	EMTableGrammar::CurrentExcelName.Empty();
	EMTableGrammar::CurrentTableName.Empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsEMPrefix
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMTableGrammar::IsEMPrefix(const FString& SheetName, const FString& PrefixName)
{
	if (SheetName.Len() < PrefixName.Len())
	{
		return false;
	}
	for (int32 i = 0; i < PrefixName.Len(); i++)
	{
		if (TChar<TCHAR>::ToUpper((*SheetName)[i]) != TChar<TCHAR>::ToUpper((*PrefixName)[i]))
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ChangeHierarchyName
// 시트명의 트리 구조명을 제거
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString EMTableGrammar::ChangeHierarchyName(const FString& HierarchyName)
{
	FString FinalName = HierarchyName.Replace(TEXT("<"), TEXT("_"));
	return FinalName.Replace(TEXT(">"), TEXT(""));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakedTypeToStructure
// 타잎명에서 스트럭쳐 이름 분해
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMTableGrammar::BreakedTypeToStructure(const FString& TypeName, TArray<TSharedPtr<FEMStructure>> OutStructures)
{
	TArray<FString> StructureTypeStrings;
	TypeName.ParseIntoArray(StructureTypeStrings, TEXT("#"));
	if (false == StructureTypeStrings[0].Equals(TEXT("struct")))
	{
		return false;
	}

	TArray<FString> NamespaceStrings;
	StructureTypeStrings[1].ParseIntoArray(NamespaceStrings, TEXT("::"));

	if (NamespaceStrings.Num() < 2)
	{
		return false;
	}

	FString NamespaceClass = NamespaceStrings[0] + TEXT("::schema::");

	TArray<FString> OutStrings;
	NamespaceStrings[1].ParseIntoArray(OutStrings, TEXT("."));
	
	TSharedPtr<FEMStructure> ParentStructure = FEMStructureManager::Inst().GetStructure(*(NamespaceClass + OutStrings[0]));
	if (false == ParentStructure.IsValid())
	{
		return false;
	}
	OutStructures.Emplace(ParentStructure);

	for (int32 i = 1; i < OutStrings.Num(); ++i)
	{
		TSharedPtr<FEMStructureMember> ChildStructureMember = ParentStructure->GetMember(*(OutStrings[i].Replace(TEXT("[]"), TEXT(""))));
		if (false == ChildStructureMember.IsValid())
		{
			return false;
		}
		TSharedPtr<FEMStructure> ChildStructure = FEMStructureManager::Inst().GetStructure(ChildStructureMember->GetType());
		if (false == ChildStructure.IsValid())
		{
			continue;
		}
		OutStructures.Emplace(ChildStructure);
		ParentStructure = ChildStructure;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsTypeToStructure
// 타잎명에서 스트럭쳐가 맞는지 판단
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EMTableGrammar::IsTypeToStructure(const FString& TypeName)
{
	TArray<FString> StructureTypeStrings;
	TypeName.ParseIntoArray(StructureTypeStrings, TEXT("#"));
	if (false == StructureTypeStrings[0].Equals(TEXT("struct")))
	{
		return false;
	}

	TArray<FString> NamespaceStrings;
	StructureTypeStrings[1].ParseIntoArray(NamespaceStrings, TEXT("::schema::"));

	if (NamespaceStrings.Num() < 2)
	{
		return false;
	}

	FString NamespaceClass = NamespaceStrings[0] + TEXT("::schema::");

	TArray<FString> OutStrings;
	NamespaceStrings[1].ParseIntoArray(OutStrings, TEXT("."));

	TSharedPtr<FEMStructure> ParentStructure = FEMStructureManager::Inst().GetStructure(*(NamespaceClass + OutStrings[0]));
	if (false == ParentStructure.IsValid())
	{
		return false;
	}
	for (int32 i = 1; i < OutStrings.Num(); ++i)
	{
		TSharedPtr<FEMStructureMember> ChildStructureMember = ParentStructure->GetMember(*(OutStrings[i].Replace(TEXT("[]"), TEXT(""))));
		if (false == ChildStructureMember.IsValid())
		{
			return false;
		}
		TSharedPtr<FEMStructure> ChildStructure = FEMStructureManager::Inst().GetStructure(ChildStructureMember->GetType());
		if (false == ChildStructure.IsValid())
		{
			continue;
		}
		ParentStructure = ChildStructure;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConvertNumToAlphabet
// 엑셀의 숫자를 알파벳번호로
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString EMTableGrammar::ConvertNumToAlphabet(int32 Column)
{
	FString Result;

	while(Column > 0)
	{
		int32 a = (Column - 1) / 26;
		int32 b = (Column - 1) % 26;

		FString Str;

		Str += TCHAR(b + 65);

		Result = Str + Result;

		Column = a;
	}
	return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PrintfException
// 예외 상황 메세지 출력에 대한 메세지 정리
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString EMTableGrammar::PrintfException(EExceptionNumber ExceptionNumber, EMTableCore::EEMSyntexException Exception, const FString& ExceptionString)
{
	FString FinalString;

	FString SheetLocation = (CurrentColumn == -1 || CurrentRow == -1) ? TEXT(":") : FString::Printf(TEXT("[$%d(%s):$%d]"), CurrentColumn + 1, *ConvertNumToAlphabet(CurrentColumn + 1), CurrentRow + 1);

	FString FinalSheetName = CurrentExcelName.Len() > 0 ? FString::Printf(TEXT("%s#%s"), *CurrentExcelName, *CurrentTableName) : CurrentTableName;

	switch (Exception)
	{
	case EMTableCore::EEMSyntexException::Error:
		FinalString = FString::Printf(TEXT("[%d]Syntax %s%s%s"), ExceptionNumber, *FinalSheetName, *SheetLocation, *ExceptionString);
		break;
	case EMTableCore::EEMSyntexException::Warning:
		FinalString = FString::Printf(TEXT("[%d]Syntax %s%s%s"), ExceptionNumber, *FinalSheetName, *SheetLocation, *ExceptionString);
		break;
	case EMTableCore::EEMSyntexException::Normal:
		FinalString = FString::Printf(TEXT("[%d] %s%s%s"), ExceptionNumber, *FinalSheetName, *SheetLocation, *ExceptionString);
		break;

	default: FinalString = ExceptionString;
	}

	return FinalString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OnException
// 예외 상황에 대한 메세지 출력및 빌드실패
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMTableGrammar::OnException(EExceptionNumber ExceptionNumber, const FString& Message, const EMTableCore::EEMSyntexException ExceptionType)
{
	EMTableCore::EEMSyntexException FinalExceptionType = ExceptionType;

	if (nullptr != EMTableConfig::Get().GetGenericSection().WarnningToErrors.Find((int32)ExceptionNumber))
	{
		FinalExceptionType = EMTableCore::EEMSyntexException::Error;
	}

	FString FinalMessage = PrintfException(ExceptionNumber, FinalExceptionType, Message);

	switch (FinalExceptionType)
	{
	case EMTableCore::EEMSyntexException::Error:
		EM_LOG(EMLog, Error, TEXT("%s"), *FinalMessage);
		if (++FatalExceptionCount >= EMTableConfig::Get().GetGenericSection().ShutdownErrorCount &&
			0 != EMTableConfig::Get().GetGenericSection().ShutdownErrorCount)
		{
			EMTableUtility::ShutdownApplication();

			exit(1);
		}
		break;
	case EMTableCore::EEMSyntexException::Warning:
		++WarningExceptionCount;
		if (nullptr == EMTableConfig::Get().GetGenericSection().DisableExceptionMsgs.Find((int32)ExceptionNumber))
		{
			EM_LOG(EMLog, Warning, TEXT("%s"), *FinalMessage);
		}
		break;
	default: EM_LOG(EMLog, Display, TEXT("%s"), *FinalMessage); break;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsValidVariableName
// 변수명으로 사용하기에 유효한지 검사
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EVariableInValidType EMTableGrammar::IsValidVariableName(const FString& VariableStr)
{
	// 첫글자에 숫자가 들어가면 변수명이 될수 없음
	if(true == EMTableCore::FEMTableString::IsFirstNumberic(*VariableStr))
	{
		return EVariableInValidType::EVariableInValidType_FirstLetterNumber;
	}
	// 사전 등록된 사용할수없는 변수명이면 사용할수 없음
	for (int32 i = 0; i < EMTableConfig::Get().GetCodeSection().InValidVariableName.Num(); i++)
	{
		if (true == VariableStr.Equals(*EMTableConfig::Get().GetCodeSection().InValidVariableName[i], ESearchCase::IgnoreCase))
		{
			return EVariableInValidType::EVariableInValidType_ReservedWords;
		}
	}
	// 영문이 아닌경우 변수명이 될수 없음
	if (false == EMTableCore::FEMTableString::IsEnglish(*VariableStr))
	{
		return EVariableInValidType::EVariableInValidType_NotWord;
	}
	// 영문이라 하더라도 공백(탭포함)은 변수명이 될수 없음
	if (false == EMTableCore::FEMTableString::IsNoSpaceEnglish(*VariableStr))
	{
		return EVariableInValidType::EVariableInValidType_WordIncludeSpace;
	}
	// 이미 Enum으로 등록됐다면 변수명이 될수 없음
	const TSharedPtr<EMCppEnumNode> Enum = EMCppParser::Get().FindEnum(*VariableStr);
	if (true == Enum.IsValid())
	{
		return EVariableInValidType::EVariableInValidType_EnumUsed;
	}

	return EVariableInValidType::EVariableInValidType_Success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsValidKey
// 키값으로 사용하기에 유효한지 검사
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EKeyInValidType EMTableGrammar::IsValidKey(EMTableCore::EEMVariabaleType Type, const FString& KeyStr)
{
	// 명확하지 않은 타잎은 키값이 될수 없음
	if (Type == EMTableCore::EEMVariabaleType::Struct ||
		Type == EMTableCore::EEMVariabaleType::Float ||
		Type == EMTableCore::EEMVariabaleType::Double ||
		Type == EMTableCore::EEMVariabaleType::Vector2 ||
		Type == EMTableCore::EEMVariabaleType::Vector3)
	{
		return EKeyInValidType::EKeyInValidType_NotSupport;
	}

	// 값이 없거나 공백(탭포함)이면 키값이 될수 없음
	if (true == EMTableCore::FEMTableString::IsEmptyWord(KeyStr) || 0 == KeyStr.Len())
	{
		return EKeyInValidType::EKeyInValidType_Empty;
	}
	if (Type == EMTableCore::EEMVariabaleType::Color)
	{
		// 컬러값을 2개 이하만 사용하거나 0~255에 있지않으면 키값으로 사용할수 없음(공백(탭)도 들어가면 안됨)
		if (false == EMTableCore::FEMTableString::IsValidType<FColor>(KeyStr))
		{
			return EKeyInValidType::EKeyInValidType_TypeNotMach;
		}
		return EKeyInValidType::EKeyInValidType_Success;
	}

	else if (EMTableCore::EEMVariabaleType::Int8 <= Type && EMTableCore::EEMVariabaleType::Uint64 >= Type)
	{
		// 숫자형태가 아닌 문자가 들어가거나 공백(탭포함)이 들어가면 키값으로 사용할수 없음
		if (false == EMTableCore::FEMTableString::IsValidNumeric(KeyStr))
		{
			return EKeyInValidType::EKeyInValidType_TypeNotMach;
		}
	}
	// 허용되지 않은 특수문자및 한글은 키값으로 사용불가(경우에 따라서는 변경될수 있음(한글포함))
	else if (false == EMTableCore::FEMTableString::IsVaildKeyString(KeyStr))
	{
		return EKeyInValidType::EKeyInValidType_TypeNotMach;
	}
	return EKeyInValidType::EKeyInValidType_Success;
}
