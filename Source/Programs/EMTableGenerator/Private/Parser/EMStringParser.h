#pragma once

#include "EMTableGenerator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMStringParseNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMStringParseNode
{
	EMStringParseNode(TWeakPtr<EMStringParseNode> InParentNode);

	TWeakPtr<EMStringParseNode> ParentNode;
	TArray<TSharedPtr<EMStringParseNode>> ChildNodes;

	int32 Start;
	int32 End;

	FString NodeString;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMStringParser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMStringParser
{
public:
	EMStringParser();
	virtual ~EMStringParser();
	
public:
	virtual void Clear();

	TSharedPtr<EMStringParseNode> ParsingSquad(
		const FString& ParseString, const TMap<TCHAR, TCHAR>& SqadDelims, int32* ResultIndex = nullptr, TSharedPtr<EMStringParseNode> ParentParserNode = nullptr, int32 Start = 0, const TCHAR Squad = 0);

	TSharedPtr<EMStringParseNode> ParsingDelims(
		const FString& ParseString, const TArray<FString>& Delims, const TArray<FString>& CommentDelims, int32* ResultIndex = nullptr, TSharedPtr<EMStringParseNode> ParentParserNode = nullptr);

	bool CollectedString(const FString& ParseString, TSharedPtr<EMStringParseNode> ParserNode, TArray<FString>& OutStrings) const;

	int64 Calculator(const FString& InString);

	static bool IsFoundDelims(const TArray<FString>& Delims, const TCHAR* String);
	static bool IsFoundDelims(const TCHAR Operator, const FString& String);

	static void ArrangeSign(TArray<FString>& InStrings);

	static bool IsSpace(const FString& String);
	static bool IsLF(const FString& String);
	static bool IsSpaceAndLF(const FString& String);
	static bool IsOperator(const TCHAR ch);
	static bool IsNaturalNumber(const FString& String);

protected:
	virtual TSharedPtr<EMStringParseNode> CreateParseNode(TSharedPtr<EMStringParseNode> ParentNode);

	TArray<FString> GetCalcStringSquad(const FString& Buffer, TSharedPtr<EMStringParseNode> ParseRootnode);
	TArray<FString> CalcStringArray(TArray<FString>& InStrings);

	bool CalcOperation(TArray<FString>& InStrings, const FString& FoundDelims);
	bool DelimContains(const FString& Delim, const FString& ParseString, int32 ParseStringIndex);

protected:
	TArray<FString> OperationDelims;
};