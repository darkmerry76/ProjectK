#include "EMCppParser.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime\Core/Public/GenericPlatform/GenericPlatformFile.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VariableTypeName
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR* VariableTypeName[] =
{
	TEXT("char"),
	TEXT("short"),
	TEXT("wchar_t"),
	TEXT("int"),
	TEXT("long"),
	TEXT("long long"),
	TEXT("__int64"),
	TEXT("unsigned char"),
	TEXT("unsigned short"),
	TEXT("unsigned wchar_t"),
	TEXT("unsigned int"),
	TEXT("unsigned long"),
	TEXT("unsigned long long"),
	TEXT("unsigned __int64"),
};

const TCHAR* VariableTypeNameUE4[] =
{
	TEXT("int8"),
	TEXT("int16"),
	TEXT("int16"),
	TEXT("int32"),
	TEXT("int32"),
	TEXT("int64"),
	TEXT("int64"),
	TEXT("uint8"),
	TEXT("uint16"),
	TEXT("uint16"),
	TEXT("uint32"),
	TEXT("uint32"),
	TEXT("uint64"),
	TEXT("uint64"),
};

#define BindCppAnalize(Command, Instance, Func )	\
	{\
		FCppAnalizeSigniture SignitureBind;\
		SignitureBind.BindRaw(Instance, Func);\
		CppAnalizesMap.Emplace(Command, SignitureBind);\
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppParser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMCppParser::EMCppParser() : EMStringParser()
{
	RootNameSpace = MakeShareable(new EMCppNameSpaceNode("#Root", nullptr));
	NameSpaces.Emplace(TEXT("#Root"), RootNameSpace);

	CommentRangeDelims.Emplace(TEXT("/*"));
	CommentRangeDelims.Emplace(TEXT("*/"));

	SyntexDelims.Emplace(TEXT("\r\n"));
	SyntexDelims.Emplace(TEXT("\r"));
	SyntexDelims.Emplace(TEXT("\n"));
	SyntexDelims.Emplace(TEXT(","));
	SyntexDelims.Emplace(TEXT("+"));
	SyntexDelims.Emplace(TEXT("-"));
	SyntexDelims.Emplace(TEXT("/"));
	SyntexDelims.Emplace(TEXT("*"));
	SyntexDelims.Emplace(TEXT(";"));
	SyntexDelims.Emplace(TEXT("=="));
	SyntexDelims.Emplace(TEXT("="));
	SyntexDelims.Emplace(TEXT("::"));
	SyntexDelims.Emplace(TEXT(":"));
	SyntexDelims.Emplace(TEXT("{"));
	SyntexDelims.Emplace(TEXT("}"));
	SyntexDelims.Emplace(TEXT("("));
	SyntexDelims.Emplace(TEXT(")"));
	SyntexDelims.Emplace(TEXT("<"));
	SyntexDelims.Emplace(TEXT(">"));
	SyntexDelims.Emplace(TEXT("["));
	SyntexDelims.Emplace(TEXT("]"));
	SyntexDelims.Emplace(TEXT("&"));
	SyntexDelims.Emplace(TEXT("^"));
	SyntexDelims.Emplace(TEXT("\\"));
	SyntexDelims.Emplace(TEXT("\""));
	SyntexDelims.Emplace(TEXT(" "));

	for (int32 i = 0; i < sizeof(VariableTypeName) / sizeof(VariableTypeName[0]); i++)
	{
		SyntexDelimsWithTypes.Emplace(VariableTypeName[i]);
	}
	for (int32 i = 0; i < sizeof(VariableTypeNameUE4) / sizeof(VariableTypeNameUE4[0]); i++)
	{
		SyntexDelimsWithTypes.Emplace(VariableTypeNameUE4[i]);
	}

	SyntexDelimsWithTypes.Append(SyntexDelims);

	VariableTypeDelims.Emplace(TEXT("::"));
	VariableTypeDelims.Emplace(TEXT("<"));
	VariableTypeDelims.Emplace(TEXT(">"));
	VariableTypeDelims.Emplace(TEXT("*"));
	VariableTypeDelims.Emplace(TEXT("]"));
	VariableTypeDelims.Emplace(TEXT("["));

	BindCppAnalize(TEXT("#define"), this, &EMCppParser::DoAnalizeDefine);
	BindCppAnalize(TEXT("enum"), this, &EMCppParser::DoAnalizeEnum);
	BindCppAnalize(TEXT("namespace"), this, &EMCppParser::DoAnalizeNameSpace);
	BindCppAnalize(TEXT("using"), this, &EMCppParser::DoAnalizeUsing);
	BindCppAnalize(TEXT("typedef"), this, &EMCppParser::DoAnalizeTypedef);
	BindCppAnalize(TEXT("class"), this, &EMCppParser::DoAnalizeClass);
	BindCppAnalize(TEXT("struct"), this, &EMCppParser::DoAnalizeStruct);

	DefaultVariableTypes.Emplace(TEXT("bool"), MakeShareable(new EMCppVariableTypeNode(TEXT("bool"), RootNameSpace.Get(), sizeof(bool))));
	DefaultVariableTypes.Emplace(TEXT("char"), MakeShareable(new EMCppVariableTypeNode(TEXT("char"), RootNameSpace.Get(), sizeof(char))));
	DefaultVariableTypes.Emplace(TEXT("int8"), MakeShareable(new EMCppVariableTypeNode(TEXT("int8"), RootNameSpace.Get(), sizeof(uint8))));
	DefaultVariableTypes.Emplace(TEXT("short"), MakeShareable(new EMCppVariableTypeNode(TEXT("short"), RootNameSpace.Get(), sizeof(short))));
	DefaultVariableTypes.Emplace(TEXT("int16"), MakeShareable(new EMCppVariableTypeNode(TEXT("int16"), RootNameSpace.Get(), sizeof(int16))));
	DefaultVariableTypes.Emplace(TEXT("int"), MakeShareable(new EMCppVariableTypeNode(TEXT("int"), RootNameSpace.Get(), sizeof(int))));
	DefaultVariableTypes.Emplace(TEXT("int32"), MakeShareable(new EMCppVariableTypeNode(TEXT("int32"), RootNameSpace.Get(), sizeof(int32))));
	DefaultVariableTypes.Emplace(TEXT("long"), MakeShareable(new EMCppVariableTypeNode(TEXT("long"), RootNameSpace.Get(), sizeof(long))));
	DefaultVariableTypes.Emplace(TEXT("int64"), MakeShareable(new EMCppVariableTypeNode(TEXT("int64"), RootNameSpace.Get(), sizeof(int64))));
	DefaultVariableTypes.Emplace(TEXT("__int64"), MakeShareable(new EMCppVariableTypeNode(TEXT("__int64"), RootNameSpace.Get(), sizeof(__int64))));
	DefaultVariableTypes.Emplace(TEXT("long long"), MakeShareable(new EMCppVariableTypeNode(TEXT("long long"), RootNameSpace.Get(), sizeof(long long))));

	DefaultVariableTypes.Emplace(TEXT("unsigned char"), MakeShareable(new EMCppVariableTypeNode(TEXT("unsigned char"), RootNameSpace.Get(), sizeof(unsigned char))));
	DefaultVariableTypes.Emplace(TEXT("uint8"), MakeShareable(new EMCppVariableTypeNode(TEXT("uint8"), RootNameSpace.Get(), sizeof(uint8))));
	DefaultVariableTypes.Emplace(TEXT("unsigned short"), MakeShareable(new EMCppVariableTypeNode(TEXT("unsigned short"), RootNameSpace.Get(), sizeof(unsigned short))));
	DefaultVariableTypes.Emplace(TEXT("uint16"), MakeShareable(new EMCppVariableTypeNode(TEXT("uint16"), RootNameSpace.Get(), sizeof(uint16))));
	DefaultVariableTypes.Emplace(TEXT("wchar_t"), MakeShareable(new EMCppVariableTypeNode(TEXT("wchar_t"), RootNameSpace.Get(), sizeof(wchar_t))));
	DefaultVariableTypes.Emplace(TEXT("unsigned int"), MakeShareable(new EMCppVariableTypeNode(TEXT("unsigned int"), RootNameSpace.Get(), sizeof(unsigned int))));
	DefaultVariableTypes.Emplace(TEXT("uint32"), MakeShareable(new EMCppVariableTypeNode(TEXT("uint32"), RootNameSpace.Get(), sizeof(uint32))));
	DefaultVariableTypes.Emplace(TEXT("unsigned long"), MakeShareable(new EMCppVariableTypeNode(TEXT("unsigned long"), RootNameSpace.Get(), sizeof(unsigned long))));
	DefaultVariableTypes.Emplace(TEXT("unsigned __int64"), MakeShareable(new EMCppVariableTypeNode(TEXT("unsigned __int64"), RootNameSpace.Get(), sizeof(unsigned __int64))));
	DefaultVariableTypes.Emplace(TEXT("uint64"), MakeShareable(new EMCppVariableTypeNode(TEXT("uint64"), RootNameSpace.Get(), sizeof(uint64))));
	DefaultVariableTypes.Emplace(TEXT("unsigned long long"), MakeShareable(new EMCppVariableTypeNode(TEXT("unsigned long long"), RootNameSpace.Get(), sizeof(unsigned long long))));
}

EMCppParser::~EMCppParser()
{
	Clear();
}

void EMCppParser::Clear()
{
	MemFile = nullptr;
	RootNameSpace = nullptr;
	CurrAnilizeNameSpace = nullptr;

	CppAnalizesMap.Empty();
	Includes.Empty();
	SyntexDelims.Empty();
	SyntexDelimsWithTypes.Empty();
	NameSpaces.Empty();
	Enums.Empty();
	FullNameStructs.Empty();
	FullNameEnums.Empty();
}

uint8* EMCppParser::GetFileBuffer(const TCHAR* Filename, int64& BufferSize)
{
	bool ExistTableFile = FPlatformFileManager::Get().GetPlatformFile().FileExists(Filename);
	if (true != ExistTableFile)
	{
		return nullptr;
	}
	IFileHandle* FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenRead(Filename);
	if (nullptr == FileHandle)
	{
		return nullptr;
	}

	BufferSize = FileHandle->Size();

	uint8* OutBuffer = (uint8*)FMemory::Malloc(FileHandle->Size() + 1);
	FMemory::Memzero(OutBuffer, FileHandle->Size() + 1);

	if (false == FileHandle->Read(OutBuffer, FileHandle->Size()))
	{
		delete FileHandle;
		return nullptr;
	}

	delete FileHandle;

	return OutBuffer;
}

bool EMCppParser::LoadFile(const TCHAR* HeaderFilename)
{
	int64 BufferSize = 0;
	uint8* Buffer = GetFileBuffer(HeaderFilename, BufferSize);
	if (nullptr == Buffer)
	{
		return false;
	}
	MemFile = MakeShareable(new EMTableCore::FEMTableMemoryFile(Buffer, BufferSize + 1));

	return true;
}

const EMTableCore::FEMTableMemoryFile& EMCppParser::GetMemeFile() const
{
	return *MemFile.Get();
}

bool EMCppParser::IsSpecialDelim(const TCHAR* String) const
{
	return IsFoundDelims(SyntexDelimsWithTypes, String);
}

bool EMCppParser::IsVariableDelim(const TCHAR* String) const
{
	return IsFoundDelims(VariableTypeDelims, String);
}

EMTableCore::EEMEnumVariableType EMCppParser::GetCppVariableType(const TArray<FString>& Tokens, int32 Index) const
{
	int32 TypeIndex = Index + 1;

	{
		int32 MaxVariableTypeName = sizeof(VariableTypeName) / sizeof(VariableTypeName[0]);

		for (int32 i = 0; i < MaxVariableTypeName; i++)
		{
			if (true == Tokens[TypeIndex].Equals(VariableTypeName[i], ESearchCase::IgnoreCase))
			{
				return (EMTableCore::EEMEnumVariableType)(i + 1);
			}
		}
	}
	{
		int32 MaxVariableTypeName = sizeof(VariableTypeNameUE4) / sizeof(VariableTypeNameUE4[0]);

		for (int32 i = 0; i < MaxVariableTypeName; i++)
		{
			if (true == Tokens[TypeIndex].Equals(VariableTypeNameUE4[i], ESearchCase::IgnoreCase))
			{
				return (EMTableCore::EEMEnumVariableType)(i + 1);
			}
		}
	}
	return EMTableCore::EEMEnumVariableType::Type_None;
}

FString EMCppParser::TransformCommentRange(const FString& TokenBufferString, const FString& EndDelim, int32& Index)
{
	FString ResultString;

	Index += 2;
	for (; Index < TokenBufferString.Len(); Index++)
	{
		if (true == DelimContains(EndDelim, TokenBufferString, Index))
		{
			Index++;
			return ResultString;
		}
		ResultString += TokenBufferString[Index];
	}
	return ResultString;
}

FString EMCppParser::TransformCommentLine(const FString& TokenBufferString, const FString& EndDelim, int32& Index)
{
	FString ResultString;

	Index += 2;
	for (; Index < TokenBufferString.Len(); Index++)
	{
		if (TokenBufferString[Index] == '\r')
		{
			return ResultString;
		}
		if ((false == DelimContains(EndDelim, TokenBufferString, Index - 1)))
		{
			ResultString += TokenBufferString[Index];
		}
	}
	return ResultString;
}

FString EMCppParser::TransformComment(const FString& TokenBufferString)
{
	FString ResultString;

	for (int32 i = 0; i < TokenBufferString.Len(); i++)
	{
		if (true == DelimContains(CommentRangeDelims[0], TokenBufferString, i))
		{
			ResultString += CommentRangeDelims[0] + TransformCommentRange(TokenBufferString, CommentRangeDelims[1], i) + CommentRangeDelims[1];
		}
		else if( true == DelimContains(TEXT("//"), TokenBufferString, i))
		{
			ResultString += CommentRangeDelims[0] + TransformCommentLine(TokenBufferString, TEXT("//"), i) + CommentRangeDelims[1];
		}
		else if (true == DelimContains(TEXT("--"), TokenBufferString, i))
		{
			ResultString += CommentRangeDelims[0] + TransformCommentLine(TokenBufferString, TEXT("--"), i) + CommentRangeDelims[1];
		}
		else
		{
			ResultString += TokenBufferString[i];
		}
	}
	return ResultString;
}

FString EMCppParser::GetFinalDefine(const FString& DefineName) const
{
	FString FinalDefineString = TEXT("");
	const EMCppDefineNode* FindDefines = RootNameSpace->Defines.Find(DefineName);
	if (nullptr != FindDefines)
	{
		for (int32 i = 0; i < FindDefines->DefineValues.Num(); i++)
		{
			FString ResultString = GetFinalDefine(*FindDefines->DefineValues[i]);
			if (FinalDefineString.Len() > 0)
			{
				FinalDefineString += TEXT(" ");
			}

			if (ResultString.Len() > 0)
			{
				FinalDefineString += ResultString;
			}
			else
			{
				FinalDefineString += FindDefines->DefineValues[i];
			}
		}
	}
	return FinalDefineString;
}

const TSharedPtr<EMCppClassNode> EMCppParser::FindClass(const EMCppNameSpaceNode* NameSpace, const FString& ClassName) const
{
	const EMCppNameSpaceNode* CurrNameSpace = NameSpace;
	if (nullptr == CurrNameSpace || CurrNameSpace->GetNodeType() != "EMCppNameSpace")
	{
		return nullptr;
	}

	const TSharedPtr<EMCppClassNode>* ResultClass = CurrNameSpace->Classes.Find(ClassName);
	if (nullptr != ResultClass)
	{
		return *ResultClass;
	}
	if (nullptr != CurrNameSpace->Parent)
	{
		return FindClass((const EMCppNameSpaceNode*)CurrNameSpace->Parent, ClassName);
	}
	return nullptr;
}

const TSharedPtr<EMCppStructNode> EMCppParser::FindStruct(const FString StructName) const
{
	const TSharedPtr<EMCppStructNode>* ResultStruct = FullNameStructs.Find(StructName);
	if (nullptr != ResultStruct)
	{
		return *ResultStruct;	
	}
	return nullptr;
}

const TSharedPtr<EMCppEnumNode> EMCppParser::FindEnum(const FString& EnumName) const
{
	const TSharedPtr<EMCppEnumNode>* EnumPtr = Enums.Find(EnumName);
	if (nullptr == EnumPtr)
	{
		return nullptr;
	}
	return *EnumPtr;
}

const TSharedPtr<EMCppEnumNode> EMCppParser::FindEnum(const EMCppNameSpaceNode* NameSpace, const FString& EnumName) const
{
	const EMCppNameSpaceNode* CurrNameSpace = NameSpace;

	if (nullptr == CurrNameSpace || CurrNameSpace->GetNodeType() != "EMCppNameSpace")
	{
		return nullptr;
	}

	const TSharedPtr<EMCppEnumNode>* EnumPtr = CurrNameSpace->Enums.Find(EnumName);
	if (nullptr != EnumPtr)
	{
		return *EnumPtr;
	}
	const TSharedPtr<EMCppClassNode>* ResultClass = CurrNameSpace->Classes.Find(EnumName);
	if (nullptr != ResultClass)
	{
		EnumPtr = ResultClass->Get()->Enums.Find(EnumName);
		if (nullptr != EnumPtr)
		{
			return *EnumPtr;
		}
	}
	if (nullptr != CurrNameSpace->Parent)
	{
		return FindEnum((const EMCppNameSpaceNode*)CurrNameSpace->Parent, EnumName);
	}
	return nullptr;
}

const TSharedPtr<EMCppEnumNode> EMCppParser::FindEnumMemberToEnum(const EMCppNameSpaceNode* NameSpace, const FString& EnumMemberName) const
{
	for (auto it = NameSpace->Enums.CreateConstIterator(); it; ++it)
	{
		const TSharedPtr<EMCppEnumNode> Enum = it->Value;

		if (nullptr != Enum->MembersMap.Find(*EnumMemberName))
		{
			return Enum;
		}
	}
	return nullptr;
}

const TSharedPtr<EMCppNameSpaceNode> EMCppParser::FindNameSpaceChild(const EMCppNameSpaceNode* NameSpace, const FString& Name) const
{
	const TSharedPtr<EMCppNameSpaceNode>* ResultFind = NameSpace->Childs.Find(*Name);
	if (nullptr != ResultFind)
	{
		return *ResultFind;
	}
	return nullptr;
}

const EMCppNameSpaceNode* EMCppParser::FindNameSpaceParent(const EMCppNameSpaceNode* NameSpace, const FString& Name) const
{
	if (nullptr == NameSpace || NameSpace->GetNodeType() != "EMCppNameSpace")
	{
		return nullptr;
	}
	if (NameSpace->Name == *Name)
	{
		return NameSpace;
	}
	return FindNameSpaceParent((const EMCppNameSpaceNode*)NameSpace->Parent, Name);
}

const FString EMCppParser::GetStringNodeName(EMCppNode* Node) const
{
	if (nullptr == Node)
	{
		return FString();
	}
	FString Result = Node->Name.Replace(TEXT("."), TEXT("::"));
	if (nullptr != Node->Parent && Node->Parent != RootNameSpace.Get())
	{
		Result = GetStringNodeName(Node->Parent) + TEXT("::") + Result;
	}
	return Result;
}

FString EMCppParser::GetFinalEnumValue(const EMCppNameSpaceNode* NameSpace, const FString& EnumName, const FString& EnumValues)
{
	TArray<FString> ParseArray;
	EnumValues.ParseIntoArray(ParseArray, TEXT(" "));

	TArray<FString> NameSpaceStrings;
	FString FinalValue;

	for (int32 i = 0; i < ParseArray.Num(); i++)
	{
		if (i + 1 < ParseArray.Num() && true == ParseArray[i + 1].Equals(TEXT("::")))
		{
			NameSpaceStrings.Emplace(ParseArray[i]);
			i += 1;
		}
		else if (true == ParseArray[i].Equals(TEXT("::")))
		{
			NameSpaceStrings.Emplace(TEXT("#Root"));
		}
		else if (true == ParseArray[i].Equals(TEXT(":")))
		{
		}
		else
		{
			TSharedPtr<EMCppEnumNode> ResultFind;
			const EMCppNameSpaceNode* CurrentNameSpace = NameSpace;

			if (0 < NameSpaceStrings.Num())
			{
				CurrentNameSpace = FindNameSpaceParent(NameSpace, NameSpaceStrings[0]);
			}
			if (false == ResultFind.IsValid())
			{
				for (int32 j = 1; j < NameSpaceStrings.Num(); j++)
				{
					if (nullptr == CurrentNameSpace)
					{
						break;
					}
					if (CurrentNameSpace->Parent->Name == *NameSpaceStrings[j])
					{
						CurrentNameSpace = CastCppNode<EMCppNameSpaceNode>(CurrentNameSpace->Parent);
					}
				}
			}
			if (nullptr != CurrentNameSpace)
			{
				ResultFind = FindEnumMemberToEnum(CurrentNameSpace, ParseArray[i]);
			}
			NameSpaceStrings.Empty();

			const TSharedPtr<EMCppEnumMemberNode>* FindEnumValue = nullptr;
			if (true == ResultFind.IsValid())
			{
				FindEnumValue = ResultFind->MembersMap.Find(*ParseArray[i]);

			}
			if (nullptr == FindEnumValue)
			{
				TSharedPtr<EMCppEnumNode> CurrentEnum = FindEnumMemberToEnum(RootNameSpace.Get(), *ParseArray[i]);
				if (true == CurrentEnum.IsValid())
				{
					FindEnumValue = CurrentEnum->MembersMap.Find(*ParseArray[i]);
				}
			}

			if (nullptr != FindEnumValue)
			{
				if (0 < FinalValue.Len())
				{
					FinalValue += TEXT(" ");
				}
				FinalValue = FString::Printf(TEXT("%d"), FindEnumValue->Get()->Value);
			}
			else
			{
				FString DefineValues = GetFinalDefine(ParseArray[i]);
				if (0 < FinalValue.Len())
				{
					FinalValue += TEXT(" ");
				}

				if (0 < DefineValues.Len())
				{
					FinalValue += DefineValues;
				}
				else
				{
					FinalValue += ParseArray[i];
				}
			}
		}
	}
	return FinalValue;
}

int32 EMCppParser::GetEnumPrefixInfo(const EMCppEnumNode& EnumNode, FString& OutPrefixString) const
{
	int32 PrefixIndex = 0;

	OutPrefixString.Empty();

	FString DefaultEnumValue = EnumNode.Name + TEXT("_None");

	if (EnumNode.Members.Num() > 1)
	{
		int32 FirstIndex = 1;
		TSharedPtr<EMCppEnumMemberNode> FirstMember = EnumNode.Members[0];
		if (EnumNode.Members.Num() > 1) {
			FirstMember = EnumNode.Members[1];
		}
		if (EnumNode.Members.Num() == 2) FirstIndex = 0;

		const FString FirstMemberName = FirstMember->Name;

		for (int i = 0; i < FirstMemberName.Len(); ++i)
		{
			TCHAR current = FirstMemberName.GetCharArray().GetData()[i];

			bool Match = true;

			for (int32 j = FirstIndex; j < EnumNode.Members.Num(); ++j)
			{
				const FString& MemberName = EnumNode.Members[j]->Name;
				if (i >= MemberName.Len())
				{
					Match = false;
					break;
				}
				if (MemberName.GetCharArray().GetData()[i] != current)
				{
					Match = false;
					break;
				}
			}
			if (Match == false)
			{
				PrefixIndex = i;
				return PrefixIndex;
			}
			else
			{
				OutPrefixString += current;
			}
		}
		PrefixIndex = FirstMemberName.Len();
	}
	return PrefixIndex;
}

FString EMCppParser::PrevComment(const TArray<FString>& Tokens, int32 Index)
{
	FString ResultString;

	return ResultString;
}

FString EMCppParser::NextComment(const TArray<FString>& Tokens, int32 Index)
{
	FString ResultString;

	for (int32 i = Index; i < Tokens.Num(); i++)
	{
		if (true == IsLF(Tokens[i]))
		{
			return ResultString;
		}
		if (true == DelimContains(CommentRangeDelims[0], Tokens[i], 0))
		{
			ResultString = &(*Tokens[i])[CommentRangeDelims[0].Len()];
			ResultString.RemoveAt(ResultString.Len() - CommentRangeDelims[1].Len(), CommentRangeDelims[1].Len());
			return ResultString;
		}
	}
	return ResultString;
}


int32 EMCppParser::DoAnalizeDefine(const TArray<FString>& Tokens, int32 Index)
{
	bool IsDefine = false;

	EMCppDefineNode CppDefine(*Tokens[Index + 1], RootNameSpace.Get());

	int32 ResultIndex = Index + 2;
	for (int32 i = Index + 2; i < Tokens.Num(); i++)
	{
		ResultIndex = i;
		if (true == IsLF(Tokens[i]))
		{
			break;
		}
		else {
			CppDefine.DefineValues.Emplace(Tokens[i]);
		}
	}

	RootNameSpace->Defines.Emplace(CppDefine.Name, CppDefine);

	return ResultIndex;
}

int32 EMCppParser::DoAnalizeEnumMembers(const TArray<FString>& Tokens, int32 Index, EMCppEnumNode* ParentEnum)
{
	check(nullptr != ParentEnum);

	bool IsNewEnumBody = true;
	bool IsNewEnumValue = false;

	TSharedPtr<EMCppEnumMemberNode> EnumValuePtr;

	FString Comment;

	int64 FinalEnumValue = 0;

	FString MergeEnumValue = FString::Printf(TEXT("%d"), FinalEnumValue);

	bool bWordReseved = false;
	for (int32 i = Index; i < Tokens.Num(); i++)
	{
		if (true == IsSpecialDelim(*Tokens[i]))
		{
			if (true == Tokens[i].Equals(TEXT("=")) && bWordReseved == false)
			{
				IsNewEnumValue = true;
				MergeEnumValue = TEXT("");

				continue;
			}
			else if (true == Tokens[i].Equals(TEXT(",")) || true == Tokens[i].Equals(TEXT("}")))
			{
				bWordReseved = false;
				if (IsNewEnumBody == false)
				{
					FString FinalEnumString = GetFinalEnumValue(CurrAnilizeNameSpace.Get(), ParentEnum->Name, MergeEnumValue);

					if(MergeEnumValue.Len() < 1)
					{
						MergeEnumValue = FString::Printf(TEXT("%d"), FinalEnumValue);
					}

					EnumValuePtr->Value = Calculator(FinalEnumString);
					EnumValuePtr->Description = Comment;

					ensure(false == ParentEnum->MembersMap.Contains(EnumValuePtr->Name));

					ParentEnum->MembersMap.Emplace(EnumValuePtr->Name, EnumValuePtr);
					ParentEnum->Members.Emplace(EnumValuePtr);

					MergeEnumValue.Empty();

					IsNewEnumBody = true;
					IsNewEnumValue = false;

					FinalEnumValue = EnumValuePtr->Value + 1;
				}
				if (true == Tokens[i].Equals(TEXT("}")))
				{
					return i;
				}
				continue;
			}
		}
		else if (true == IsNewEnumBody)
		{
			EnumValuePtr = MakeShareable(new EMCppEnumMemberNode(*Tokens[i], ParentEnum));
			Comment = NextComment(Tokens, i + 1);
			IsNewEnumBody = false;
		}

		if (true == IsNewEnumValue)
		{
			if (0 < MergeEnumValue.Len())
			{
				MergeEnumValue += TEXT(" ");
			}
			
			if(false == bWordReseved)
			{
				MergeEnumValue += Tokens[i];
			}
			if ((i + 1 < Tokens.Num()) && Tokens[i + 1].Len() > 0 && false == IsOperator(Tokens[i + 1][0]) && false == IsOperator(Tokens[i][0]))
			{
				bWordReseved = true;
			}
		}
	}
	return Tokens.Num() - 1;
}

int32 EMCppParser::DoAnalizeEnum(const TArray<FString>& Tokens, int32 Index)
{
	TSharedPtr<EMCppEnumNode> ResultEnum = nullptr;

	for (int32 i = Index + 1; i < Tokens.Num(); i++)
	{
		FString SpecualDelum = Tokens[i];
		if (true == IsSpecialDelim(*Tokens[i]))
		{
			if (true == Tokens[i].Equals(TEXT(":")))
			{
				if (true == ResultEnum.IsValid())
				{
					ResultEnum->VariableType = GetCppVariableType(Tokens, i);
				}
			}
			else if (true == Tokens[i].Equals(TEXT("{")))
			{
				check(true == ResultEnum.IsValid());
				i = DoAnalizeEnumMembers(Tokens, i, ResultEnum.Get());

				GetEnumPrefixInfo(*ResultEnum, ResultEnum->Prefix);

				return i;
			}
		}
		else if (true == Tokens[i].Equals(TEXT("class")))
		{
		}
		else if(false == ResultEnum.IsValid())
		{
			ResultEnum = MakeShareable(new EMCppEnumNode(*Tokens[i], CurrAnilizeNameSpace.Get()));

			AddEnum(CurrAnilizeNameSpace.Get(), ResultEnum.Get());
		}
	}
	return Tokens.Num() - 1;
}

void EMCppParser::AddEnum(EMCppNameSpaceNode* NameSpaceNode, EMCppEnumNode* EnumNode)
{
	NameSpaceNode->Enums.Emplace(EnumNode->Name, StaticCastSharedRef<EMCppEnumNode>(EnumNode->AsShared()));
	Enums.Emplace(EnumNode->Name, StaticCastSharedRef<EMCppEnumNode>(EnumNode->AsShared()));

	const FString FullNodeName = GetVariableTypeFullName(EnumNode, EnumNode->Name);

	FullNameEnums.Emplace(FullNodeName, StaticCastSharedRef<EMCppEnumNode>(EnumNode->AsShared()));
}

int32 EMCppParser::DoAnalizeClass(const TArray<FString>& Tokens, int32 Index)
{
	return Index;
}

void EMCppParser::CollectionParentNode(const EMCppNode* CurrNode, TArray<EMCppNode*>& OutResults) const
{
	if (nullptr == CurrNode || nullptr == CurrNode->Parent)
	{
		return;
	}

	OutResults.Emplace(CurrNode->Parent);

	CollectionParentNode(CurrNode->Parent, OutResults);
}

const FString EMCppParser::GetVariableTypeFullName(const EMCppNode* CurrNode, const FString& VaribleTypeName) const
{
	TArray<EMCppNode*> OutResults;
	CollectionParentNode(CurrNode, OutResults);

	FString FinalName;
	
	for (int32 i = OutResults.Num() - 1; i >= 0; i--)
	{
		check(nullptr != OutResults[i]);
		
		if(RootNameSpace.Get() == OutResults[i]) continue;

		FinalName += OutResults[i]->Name;
		FinalName += TEXT("::");
	}

	FinalName += VaribleTypeName;

	return FinalName;
}

const EMCppNode* EMCppParser::FindValidVariableType(const EMCppNode* CurrNode, const EMCppTypeNode& Type) const
{
	if (nullptr == CurrNode)
	{
		return nullptr;
	}

	const FString FinalTypeName = GetVariableTypeFullName(CurrNode, Type.Name);

	const EMCppNode* ResultTypeNode = FindTypeNode(FinalTypeName);
	if (nullptr == ResultTypeNode)
	{
		const EMCppNode* ParentResultTypeNode = FindValidVariableType(CurrNode->Parent, Type);
		if (nullptr != ParentResultTypeNode)
		{
			return ParentResultTypeNode;
		}
	}
	return ResultTypeNode;
}

FString EMCppParser::GetPrevVariableName(const TArray<FString>& Tokens, int32 Index, int32* OutIndex) const
{
	while(true == IsSpecialDelim(*Tokens[--Index]) && Index > 0);

	if(nullptr != OutIndex)
	{
		*OutIndex = Index;
	}

	return Tokens[Index];
}

FString EMCppParser::GetPrevVariableTypeName(const TArray<FString>& Tokens, int32 Index, int32* OutIndex) const
{
	FString OutVariable;
	TArray<int32> ParseIndices;
	for (int32 i = Index - 1; i >= 0; i--)
	{
		if (true == IsSpecialDelim(*Tokens[i]) && nullptr == DefaultVariableTypes.Find(Tokens[i]))
		{
			if (false == IsVariableDelim(*Tokens[i]))
			{
				if (nullptr != OutIndex)
				{
					*OutIndex = i;
				}
				break;
			}
		}
		ParseIndices.Emplace(i);
	}
	for (int32 i = ParseIndices.Num() - 1; i >= 0; i--)
	{
		
		if (i + 1 < ParseIndices.Num() && false == IsSpecialDelim(*Tokens[ParseIndices[i + 1]]) && false == IsSpecialDelim(*Tokens[ParseIndices[i]]))
		{
			OutVariable += TEXT(" ");
		}
		OutVariable += Tokens[ParseIndices[i]];
	}

	return OutVariable;
}

bool EMCppParser::IsPrevFunction(const TArray<FString>& Tokens, int32 Index, int32* OutIndex) const
{
	int32 SquadCount =0;
	for (int32 i = Index - 1; i >= 0; i--)
	{
		if (true == Tokens[i].Equals(TEXT(")")))
		{
			SquadCount++;
		}
		else if (true == Tokens[i].Equals(TEXT("(")))
		{
			if (--SquadCount == 0)
			{
				if (nullptr != OutIndex)
				{
					*OutIndex = i;
				}
				return true;
			}
		}
		else if (true == Tokens[i].Equals(TEXT("const")))
		{

		}
		else if (true == Tokens[i].Equals(TEXT("override")))
		{

		}
		else if (true == Tokens[i].Equals(TEXT("final")))
		{

		}
		else if (true == Tokens[i].Equals(TEXT("{")))
		{

		}
		else if (true == Tokens[i].Equals(TEXT("}")))
		{

		}
		else
		{
			break;
		}
	}
	return false;
}

int32 EMCppParser::DoAnalizeStruct(const TArray<FString>& Tokens, int32 Index)
{
	TSharedPtr<EMCppStructNode> NewStruct;

	int32 SquadDepth =0;
	for (int32 i = Index + 1; i < Tokens.Num(); i++)
	{
		if (true == IsSpecialDelim(*Tokens[i]))
		{
			if (Tokens[i].Equals(TEXT("{")))
			{
				if(false == NewStruct.IsValid())
				{
					NewStruct = MakeShareable(new EMCppStructNode(GetPrevVariableName(Tokens, i), CurrAnilizeNameSpace.Get()));
				}
				SquadDepth++;
			}
			else if (Tokens[i].Equals(TEXT("}")))
			{
				if (--SquadDepth <= 0)
				{
					break;
				}
			}
			else if (Tokens[i].Equals(TEXT(";")))
			{
				int32 NameTokenIndex = i;
				int32 FunctionTokenIndex = 0;

				if (true == NewStruct->Name.Contains(TEXT("AutoAIInfoJson")))
				{
					NameTokenIndex = NameTokenIndex;
				}

				TSharedPtr<EMCppMemberVariable> NewMemberVariable;
				bool IsFunction = IsPrevFunction(Tokens, i, &FunctionTokenIndex);
				if(true == IsFunction)
				{
					NewMemberVariable =
						MakeShareable(new EMCppMemberVariable(GetPrevVariableName(Tokens, FunctionTokenIndex, &NameTokenIndex), NewStruct.Get()));

					NewMemberVariable->Type = MakeShareable(new EMCppVariableTypeNode(GetPrevVariableTypeName(Tokens, NameTokenIndex), NewMemberVariable.Get()));
				}
				else
				{
					NewMemberVariable =
						MakeShareable(new EMCppMemberVariable(GetPrevVariableName(Tokens, i, &NameTokenIndex), NewStruct.Get()));					

					NewMemberVariable->Type = MakeShareable(new EMCppVariableTypeNode(GetPrevVariableTypeName(Tokens, NameTokenIndex), NewMemberVariable.Get()));
				}
				NewStruct->Members.Emplace(NewMemberVariable->Name, NewMemberVariable);
			}
		}
	}
	if(true == NewStruct.IsValid())
	{
		FullNameStructs.Emplace(GetStringNodeName(NewStruct.Get()), NewStruct);
	}
	return Index;
}

int32 EMCppParser::DoAnalizeTypedef(const TArray<FString>& Tokens, int32 Index)
{
	return Index;
}

int32 EMCppParser::DoAnalizeUsing(const TArray<FString>& Tokens, int32 Index)
{
	for (int32 i = Index + 1; i < Tokens.Num(); i++)
	{
		if (true == IsSpecialDelim(*Tokens[i]))
		{
			if (true == Tokens[i].Equals(TEXT(";")))
			{
				return i;
			}
		}
	}
	return Tokens.Num();
}

int32 EMCppParser::DoAnalizeNameSpace(const TArray<FString>& Tokens, int32 Index)
{
	TSharedPtr<EMCppNameSpaceNode> NewNameSpace;

	TSharedPtr<EMCppNameSpaceNode>* FindNameSpace = CurrAnilizeNameSpace->Childs.Find(*Tokens[Index + 1]);
	if (nullptr != FindNameSpace)
	{
		NewNameSpace = *FindNameSpace;
	}
	else
	{
		NewNameSpace = MakeShareable(new EMCppNameSpaceNode(*Tokens[Index + 1], CurrAnilizeNameSpace.Get()));
		CurrAnilizeNameSpace->Childs.Emplace(*Tokens[Index + 1], NewNameSpace);
	}

	TSharedPtr<EMCppNameSpaceNode> RememberAnilizeNameSpace = CurrAnilizeNameSpace;

	CurrAnilizeNameSpace = NewNameSpace;

	int32 ResultIndex = Index + 2;
	for (int32 i = Index + 2; i < Tokens.Num(); i++)
	{
		if (true == IsSpecialDelim(*Tokens[i]))
		{
			if (true == Tokens[i].Equals(TEXT("}")))
			{
				return i;
			}
		}

		FCppAnalizeSigniture* Signiture = CppAnalizesMap.Find(Tokens[i]);
		if (nullptr != Signiture)
		{
			i = Signiture->Execute(Tokens, i);
		}
		ResultIndex = i;
	}

	CurrAnilizeNameSpace = RememberAnilizeNameSpace;

	return ResultIndex;
}

const EMCppNode* EMCppParser::FindTypeNode(const FString& TypeFullName) const
{
	const TSharedPtr<EMCppStructNode>* ResultStruct = FullNameStructs.Find(TypeFullName);
	if (nullptr != ResultStruct)
	{
		return ResultStruct->Get();
	}
	const TSharedPtr<EMCppEnumNode>* ResultEnum = FullNameEnums.Find(TypeFullName);
	if (nullptr != ResultEnum)
	{
		return ResultEnum->Get();
	}
	const TSharedPtr<EMCppVariableTypeNode>* ResultVariable = DefaultVariableTypes.Find(TypeFullName);
	if (nullptr != ResultVariable)
	{
		return ResultVariable->Get();
	}
	return nullptr;
}

void EMCppParser::FinalAnalize()
{
	for (auto StructIt = FullNameStructs.CreateIterator(); StructIt; ++StructIt)
	{
		check(true == StructIt->Value.IsValid());

		EMCppStructNode* StructNode = StructIt->Value.Get();
		if (true == StructNode->Name.Contains(TEXT("AutoAIInfoJson")))
		{
			StructNode = StructNode;
		}

		for(auto StructMemberIt = StructNode->Members.CreateIterator(); StructMemberIt; ++StructMemberIt)
		{
			check(true == StructMemberIt->Value.IsValid() && true == StructMemberIt->Value->Type.IsValid());

			EMCppTypeNode* Type = StructMemberIt->Value->Type.Get();

			const EMCppNode* ResultTypeNode = FindValidVariableType(StructNode, *Type);
			if (nullptr != ResultTypeNode)
			{
				Type->Target = ConstCastSharedRef<EMCppNode>(ResultTypeNode->AsShared());
			}
		}		
	}
}

bool EMCppParser::DoAnalize(const TCHAR* TokenBuffer, int32 BufferSize)
{
	if (TokenBuffer[0] == 0xFEFF) // ByteOfMark
	{
		TokenBuffer++;
	}

	FString StringTokenBuffer = TransformComment(TokenBuffer);

	StringTokenBuffer = StringTokenBuffer.Replace(TEXT("\t"), TEXT(" "));

	for (int32 i = 0; i < StringTokenBuffer.Len(); i++)
	{
		if ((*StringTokenBuffer)[i] == ' ')
		{
			while (i + 1 < StringTokenBuffer.Len() && (*StringTokenBuffer)[i + 1] == ' ')
			{
				StringTokenBuffer.RemoveAt(i + 1);
			}
			i++;
		}
	}
	TArray<FString> SyntexTokens;

	TSharedPtr<EMStringParseNode> SyntexRootnode = ParsingDelims(StringTokenBuffer, SyntexDelims, CommentRangeDelims);
	CollectedString(StringTokenBuffer, SyntexRootnode, SyntexTokens);

	for (int32 i = 0; i < SyntexTokens.Num(); i)
	{
		if (true == SyntexTokens[i].Equals(TEXT(" ")))
		{
			SyntexTokens.RemoveAt(i);
		}
		else i++;
	}
	for (int32 i = 0; i < SyntexTokens.Num(); i++)
	{
		if (true == SyntexTokens[i].Equals(TEXT("\"")))
		{
			while (i + 1 < SyntexTokens.Num() && false == SyntexTokens[i + 1].Equals(TEXT("\""))) {
				SyntexTokens[i] += SyntexTokens[i + 1];
				SyntexTokens.RemoveAt(i + 1);
			};
			if (i + 1 < SyntexTokens.Num())
			{
				SyntexTokens[i] += SyntexTokens[i + 1];
				SyntexTokens.RemoveAt(i + 1);
			}
			i++;
		}
	}

	CurrAnilizeNameSpace = RootNameSpace;
	for (int32 i = 0; i < SyntexTokens.Num(); i++)
	{
		FCppAnalizeSigniture* Signiture = CppAnalizesMap.Find(SyntexTokens[i]);
		if (nullptr != Signiture)
		{
			i = Signiture->Execute(SyntexTokens, i);
		}
	}
	return true;
}