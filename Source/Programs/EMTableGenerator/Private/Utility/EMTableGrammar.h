#pragma once

#include "EMTableGenerator.h"
#include "SheetBuilder/EMStructure.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EKeyInValidType
////////////////////////////////////////////////////////////////////////////////////////////////////
enum class EKeyInValidType : uint8
{
	EKeyInValidType_Success,
	EKeyInValidType_Empty,
	EKeyInValidType_TypeNotMach,
	EKeyInValidType_NotSupport,
	EKeyInValidType__Max,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// EVariableInValidType
////////////////////////////////////////////////////////////////////////////////////////////////////
enum class EVariableInValidType : uint8
{
	EVariableInValidType_Success,
	EVariableInValidType_NotWord,
	EVariableInValidType_WordIncludeSpace,
	EVariableInValidType_FirstLetterNumber,
	EVariableInValidType_ReservedWords,
	EVariableInValidType_EnumUsed,
	EVariableInValidType_Max
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// EExceptionNumber
////////////////////////////////////////////////////////////////////////////////////////////////////
enum class EExceptionNumber : int32
{
	EExceptionNumber_Variable_InVaild					= 10001,	// 변수타잎이 유효하지 않음
	EExceptionNumber_Variable_Empty						= 10002,	// 변수값이 비어있음

	EExceptionNumber_SheetName_Short					= 11001,	// Sheet이름이 너무 짧음
	EExceptionNumber_SheetName_NotWord					= 11002,	// Sheet이름에는 문자만 들어가야함
	EExceptionNumber_SheetName_IncludeSpaceWord			= 11003,	// Sheet이름에는 띄어쓰기가 포함될수 없음
	EExceptionNumber_SheetName_FirstLetterNumber		= 11004,	// Sheet이름 첫글자에 숫자가 올수 없음
	EExceptionNumber_SheetName_UseReservedWord			= 11005,	// 사용할수 없는 변수명을 사용했음(C++에서 사용되는 int,char등은 변수명으로 사용할수 없음)
	EExceptionNumber_MetaSheetName_InVaild				= 11006,	// 메타 Sheet와 매칭되는 Sheet가 없음

	EExceptionNumber_VariableName_Short					= 12001,	// 변수이름이 너무 짧음
	EExceptionNumber_VariableName_NotWord				= 12002,	// 변수이름에는 문자만 들어가야함
	EExceptionNumber_VariableName_IncludeSpaceWord		= 12003,	// 변수이름에는 띄어쓰기가 포함될수 없음
	EExceptionNumber_VariableName_FirstCharacterNumber	= 12004,	// 변수이름 첫글자에 숫자가 올수 없음
	EExceptionNumber_VariableName_UseReservedWord		= 12005,	// 사용할수 없는 변수명을 사용했음(C++에서 사용되는 int,char등은 변수명으로 사용할수 없음)
	EExceptionNumber_VariableName_UseEnum				= 12006,	// Enum으로 사용중인 변수명은 사용할수 없습니다.
	EExceptionNumber_VariableName_UnkownError			= 12009,	// 알수없는 변수명 에러

	EExceptionNumber_Variable_HasTheSameStructure		= 13001,	// 서로같은 시트정보를 사용하지만 변수구조가 다름 (예:EM_NPC<Monster>, EM_NPC<NPC> 두 시트중에 이름은 같지만 타잎이 틀린경우 int name, uint64 name)
	EExceptionNumber_Variable_TypeUnknown				= 13002,	// 알수 없는 변수타잎
	EExceptionNumber_Variable_KeyNull					= 13003,	// 키값이 비어있음
	EExceptionNumber_Variable_KeyEmptyWordString		= 13004,	// 키값에 비어있음(Tab이나 띄어쓰기는 빈값으로 취급)
	EExceptionNumber_Variable_KeyInvalidType			= 13005,	// 키값이 유효하지 않음
	EExceptionNumber_Variable_NotSupportType			= 13006,	// 키값으로 사용할수 없는 타잎

	EExceptionNumber_SheetBuilder_EmptyMemory			= 14001,	// SheetBuilder를 실행중 메모리가 모자름

	EExceptionNumber_NotFoundVariableEnum				= 15001,	// Enum값을 찾을수 없음
	EExceptionNumber_NotFoundEnumMember					= 15002,	// Enum멤버를 찾을수 없음
	EExceptionNumber_NotConvertEnumTypeVariableType		= 15003,

	EExceptionNumber_Key_ArrayInvalid					= 16001,	// 키값에는 배열을 사용할수 없음
	EExceptionNumber_Key_Exist							= 16002,	// 키값이 중복
	EExceptionNumber_Key_EmptyAuto						= 16003,	// 키값이 없어서 자동생성됨

	EExceptionNumber_EnumOverrap						= 17001,	// Enum이 중복됨
	EExceptionNumber_EnumUknownType						= 17002,	// 알수없는 EnumType
	EExceptionNumber_EnumNotValueColumn					= 17003,	// Enum Value컬럼이 없습니다

	EExceptionNumber_ClassPathInValid					= 18001,	// 언리얼 클래쓰 패쓰 형식이 틀렸음
	EExceptionNumber_AssetPathInValid					= 18002,	// 어셋 패쓰 형식이 틀렸음

	EExceptionNumber_CppParsing_EnumMemberOverlap		= 19001,	// Cpp 파싱중 EnumMember가 겹침
	
};
////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableGrammar
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableGrammar
{
public:
	static bool IsEMPrefix(const FString& SheetName, const FString& PrefixName);

	static FString ConvertNumToAlphabet(int32 Column);
	static void ResetExceptionInfo();

	static FString ChangeHierarchyName(const FString& HierarchyName);
	static bool BreakedTypeToStructure(const FString& TypeName, TArray<TSharedPtr<FEMStructure>> OutStructures);
	static bool IsTypeToStructure(const FString& TypeName);

	static EVariableInValidType IsValidVariableName(const FString& VariableStr);
	static EKeyInValidType IsValidKey(EMTableCore::EEMVariabaleType Type, const FString& KeyStr);

	static void OnException(EExceptionNumber ExceptionNumber, const FString& Message, const EMTableCore::EEMSyntexException ExceptionType);
	static FString PrintfException(EExceptionNumber ExceptionNumber, EMTableCore::EEMSyntexException Exception, const FString& ExceptionString);

	static int32	CurrentRow;
	static int32	CurrentColumn;
	static FString	CurrentExcelName;
	static FString	CurrentTableName;
	static int32	FatalExceptionCount;
	static int32	WarningExceptionCount;
};