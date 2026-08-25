#pragma once

#include "EMTableGenerator.h"
#include "EMStringParser.h"

enum class eEnumOnwerType
{
	eEnumOnwerType_Cpp,
	eEnumOnwerType_Excel,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppNode : public TSharedFromThis<EMCppNode>
{
public:
	EMCppNode() : Parent(nullptr), NodeType(GetStaticNodeType()) { };
	EMCppNode(FString InName, EMCppNode* InNode) : Parent(InNode), NodeType(GetStaticNodeType()), Name(InName), OwnerType(eEnumOnwerType::eEnumOnwerType_Cpp) { };

	FName GetNodeType() const { return NodeType; };

	static FName GetStaticNodeType() { return "EMCppNode"; };

	EMCppNode* Parent;
	FName NodeType;
	FString Name;
	FString Description;
	eEnumOnwerType OwnerType;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CastCppNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename _TL>
_TL* CastCppNode(class EMCppNode* CppNode)
{
	if (nullptr == CppNode)
	{
		return nullptr;
	}
	if (CppNode->GetNodeType() == _TL::GetStaticNodeType())
	{
		return (_TL*) CppNode;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMIncludeNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMIncludeNode : public EMCppNode
{
public:
	EMIncludeNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent) { NodeType = GetStaticNodeType(); };

	static FName GetStaticNodeType() { return "EMIncludeNode"; };

	FString	IncludeFile;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppDefineNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppDefineNode : public EMCppNode
{
public:
	EMCppDefineNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent) { NodeType = GetStaticNodeType(); };

	TArray<FString>		DefineValues;

	static FName GetStaticNodeType() { return "EMCppDefineNode"; };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppEnumMemberNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppEnumMemberNode : public EMCppNode
{
public:
	EMCppEnumMemberNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent) {
		NodeType = GetStaticNodeType();
	};

	static FName GetStaticNodeType() { return "EMCppEnumMemberNode"; };

	int64		Value;
	FString		DisplayName;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppEnum
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppEnumNode : public EMCppNode
{
public:
	EMCppEnumNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent)
		, VariableType(EMTableCore::EEMEnumVariableType::Int32)
		, IsClass(false) { NodeType = GetStaticNodeType(); };

	static FName GetStaticNodeType() { return "EMCppEnumNode"; };

	const TSharedPtr<EMCppEnumMemberNode> FindMember(const FString& MemberName) const
	{
		const TSharedPtr<EMCppEnumMemberNode>* EnumMemberNode = MembersMap.Find(MemberName);
		if (nullptr == EnumMemberNode)
		{
			return nullptr;
		}
		return *EnumMemberNode;
	}

	TMap<FString, TSharedPtr<EMCppEnumMemberNode>>	MembersMap;
	TArray<TSharedPtr<EMCppEnumMemberNode>>	Members;

	EMTableCore::EEMEnumVariableType	VariableType;

	FString Prefix;

	bool IsClass;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppTypeNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppTypeNode : public EMCppNode
{
public:
	EMCppTypeNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent), Target(nullptr)
	{
		NodeType = GetStaticNodeType();
	};
	static FName GetStaticNodeType() { return "EMCppTypeNode"; };

	TWeakPtr<EMCppNode> Target;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppVariableTypeNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppVariableTypeNode : public EMCppTypeNode
{
public:
	EMCppVariableTypeNode(FString InName, EMCppNode* InParent, int InTypeSize = 0) : EMCppTypeNode(InName, InParent), ArrayCount(-1), TypeSize(InTypeSize)
	{
		NodeType = GetStaticNodeType();
	}
	static FName GetStaticNodeType() { return "EMCppVariableTypeNode"; };

	int32 ArrayCount;
	int32 TypeSize;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppFunctionTypeNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppFunctionTypeNode : public EMCppTypeNode
{
public:
	EMCppFunctionTypeNode(FString InName, EMCppNode* InParent) : EMCppTypeNode(InName, InParent)
	{
		NodeType = GetStaticNodeType();
	}
	static FName GetStaticNodeType() { return "EMCppFunctionTypeNode"; };

	TArray<TSharedPtr<EMCppTypeNode>> Parameters;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppMemberVariable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppMemberVariable : public EMCppNode
{
public:
	EMCppMemberVariable(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent)
	{
		NodeType = GetStaticNodeType();
	};
	static FName GetStaticNodeType() { return "EMCppMemberVariable"; };

	TSharedPtr<EMCppTypeNode> Type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppClassNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppClassNode : public EMCppNode
{
public:
	EMCppClassNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent) { NodeType = GetStaticNodeType(); };
	static FName GetStaticNodeType() { return "EMCppClassNode"; };

	TMap<FString, TSharedPtr<EMCppEnumNode>> Enums;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppStructNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppStructNode : public EMCppNode
{
public:
	EMCppStructNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent) { NodeType = GetStaticNodeType(); };
	static FName GetStaticNodeType() { return "EMCppStructNode"; };

	TMap <FString, TSharedPtr<EMCppMemberVariable>> Members;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCppNameSpaceNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppNameSpaceNode : public EMCppNode
{
public:
	EMCppNameSpaceNode(FString InName, EMCppNode* InParent) : EMCppNode(InName, InParent) { NodeType = GetStaticNodeType(); };
	static FName GetStaticNodeType() { return "EMCppNameSpaceNode"; };

	TMap<FString, TSharedPtr<EMCppNameSpaceNode>> Childs;
	TMap<FString, TSharedPtr<EMCppClassNode>> Classes;
	TMap<FString, TSharedPtr<EMCppEnumNode>> Enums;
	TMap<FString, EMCppDefineNode>	Defines;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMStringParseNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCppParser : public EMStringParser
{
public:
	EMCppParser();
	virtual ~EMCppParser();

public:
	static EMCppParser& Get()
	{
		static EMCppParser Instance;
		return Instance;
	}

public:

	virtual void Clear();

	bool LoadFile(const TCHAR* HeaderFilename);

	bool DoAnalize(const TCHAR* TokenBuffer, int32 BufferSize);

	const EMTableCore::FEMTableMemoryFile& GetMemeFile() const;

	const TSharedPtr<EMCppNameSpaceNode> GetRootNameSpace() const { return RootNameSpace; };

	const TSharedPtr<EMCppClassNode> FindClass(const EMCppNameSpaceNode* NameSpace, const FString& ClassName) const;
	const TSharedPtr<EMCppStructNode> FindStruct(const FString StructName) const;

	const TSharedPtr<EMCppEnumNode> FindEnum(const FString& EnumName) const;
	const TSharedPtr<EMCppEnumNode> FindEnum(const EMCppNameSpaceNode* NameSpace, const FString& EnumName) const;

	const TMap<FString, TSharedPtr<EMCppEnumNode>>& GetEnums() const { return Enums; };
	TMap<FString, TSharedPtr<EMCppEnumNode>>& GetMutableEnums() { return Enums; };

	const TSharedPtr<EMCppEnumNode> FindEnumMemberToEnum(const EMCppNameSpaceNode* NameSpace, const FString& EnumMemberName) const;
	const TSharedPtr<EMCppNameSpaceNode> FindNameSpaceChild(const EMCppNameSpaceNode* NameSpace, const FString& Name) const;
	const EMCppNameSpaceNode* FindNameSpaceParent(const EMCppNameSpaceNode* NameSpace, const FString& Name) const;

	const FString GetStringNodeName(EMCppNode* Node) const;

	const EMCppNode* FindTypeNode(const FString& TypeFullName) const;

	void FinalAnalize();

protected:
	FString GetFinalDefine(const FString& DefineName) const;
	FString GetFinalEnumValue(const EMCppNameSpaceNode* NameSpace, const FString& EnumName, const FString& EnumValues);

	uint8* GetFileBuffer(const TCHAR* Filename, int64& BufferSize);

	FString TransformComment(const FString& TokenBufferString);
	FString TransformCommentLine(const FString& TokenBufferString, const FString& EndDelim, int32& Index);
	FString TransformCommentRange(const FString& TokenBufferString, const FString& EndDelim, int32& Index);

	FString PrevComment(const TArray<FString>& Tokens, int32 Index);
	FString NextComment(const TArray<FString>& Tokens, int32 Index);

	int32 DoAnalizeDefine(const TArray<FString>& Tokens, int32 Index);
	int32 DoAnalizeEnum(const TArray<FString>& Tokens, int32 Index);
	int32 DoAnalizeEnumMembers(const TArray<FString>& Tokens, int32 Index, EMCppEnumNode* ParentEnum);
	int32 DoAnalizeClass(const TArray<FString>& Tokens, int32 Index);
	int32 DoAnalizeStruct(const TArray<FString>& Tokens, int32 Index);
	int32 DoAnalizeTypedef(const TArray<FString>& Tokens, int32 Index);
	int32 DoAnalizeNameSpace(const TArray<FString>& Tokens, int32 Index);
	int32 DoAnalizeUsing(const TArray<FString>& Tokens, int32 Index);
	int32 DoAnalizeUsingNameSpace(const TArray<FString>& Tokens, int32 Index);

	void AddEnum(EMCppNameSpaceNode* NameSpaceNode, EMCppEnumNode* EnumNode);

	EMTableCore::EEMEnumVariableType GetCppVariableType(const TArray<FString>& Tokens, int32 Index) const;

	int32 GetEnumPrefixInfo(const EMCppEnumNode& EnumNodePtr, FString& OutPrefixString) const;
	bool IsSpecialDelim(const TCHAR* String) const;
	bool IsVariableDelim(const TCHAR* String) const;

	bool IsPrevFunction(const TArray<FString>& Tokens, int32 Index, int32* OutIndex = nullptr) const;

	FString GetPrevVariableName(const TArray<FString>& Tokens, int32 Index, int32* OutIndex = nullptr) const;
	FString GetPrevVariableTypeName(const TArray<FString>& Tokens, int32 Index, int32* OutIndex = nullptr) const;

	const FString GetVariableTypeFullName(const EMCppNode* CurrNode, const FString& VaribleTypeName) const;
	const EMCppNode* FindValidVariableType(const EMCppNode* CurrNode, const EMCppTypeNode& Type) const;
	void CollectionParentNode(const EMCppNode* CurrNode, TArray<EMCppNode*>& OutResults) const;

protected:
	DECLARE_DELEGATE_RetVal_TwoParams(int32, FCppAnalizeSigniture, const TArray<FString>&, int32)

	TMap<FString, TSharedPtr<EMCppVariableTypeNode>> DefaultVariableTypes;

	TMap<FString,FCppAnalizeSigniture>	CppAnalizesMap;
	TMap<FString, TSharedPtr<EMCppStructNode>> FullNameStructs;
	TSharedPtr<EMTableCore::FEMTableMemoryFile>	MemFile;

	TMap<FString, EMIncludeNode>	Includes;

	TArray<FString> SyntexDelims;
	TArray<FString> SyntexDelimsWithTypes;

	TArray<FString> VariableTypeDelims;

	TSharedPtr<EMCppNameSpaceNode> RootNameSpace;

	TMap<FString, TSharedPtr<EMCppNameSpaceNode>> NameSpaces;
	TMap<FString, TSharedPtr<EMCppEnumNode>> Enums;
	TMap<FString, TSharedPtr<EMCppEnumNode>> FullNameEnums;

	TSharedPtr<EMCppNameSpaceNode> CurrAnilizeNameSpace;
	TArray<FString> CommentRangeDelims;
};