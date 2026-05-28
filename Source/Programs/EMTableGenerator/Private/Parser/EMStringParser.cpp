#include "EMStringParser.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMStringParseNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMStringParseNode::EMStringParseNode(TWeakPtr<EMStringParseNode> InParentNode) : ParentNode(InParentNode)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMStringParse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMStringParser::EMStringParser()
{
	OperationDelims.Emplace(TEXT("%"));
	OperationDelims.Emplace(TEXT("/"));
	OperationDelims.Emplace(TEXT("*"));
	OperationDelims.Emplace(TEXT("-"));
	OperationDelims.Emplace(TEXT("+"));
}

EMStringParser::~EMStringParser()
{
	Clear();
}

void EMStringParser::Clear()
{
	OperationDelims.Empty();
}

TSharedPtr<EMStringParseNode> EMStringParser::CreateParseNode(TSharedPtr<EMStringParseNode> ParentNode)
{
	TSharedPtr<EMStringParseNode> ParserNode = MakeShareable(new EMStringParseNode(ParentNode));
	if (true == ParentNode.IsValid())
	{
		ParentNode->ChildNodes.Emplace(ParserNode);
	}
	return ParserNode;
}


bool EMStringParser::CollectedString(const FString& ParseString, TSharedPtr<EMStringParseNode> ParserNode, TArray<FString>& OutStrings) const
{
	if (0 == ParserNode->ChildNodes.Num())
	{
		int32 Length = ParserNode->End - ParserNode->Start;
		FString LeafString;
		LeafString.Append(&(*ParseString)[ParserNode->Start], Length);
		OutStrings.Emplace(LeafString);
	}

	for (int32 i = 0; i < ParserNode->ChildNodes.Num(); i++)
	{
		if (false == CollectedString(ParseString, ParserNode->ChildNodes[i], OutStrings))
		{
			return false;
		}
	}
	return true;
}

TSharedPtr<EMStringParseNode> EMStringParser::ParsingSquad(
	const FString& ParseString, const TMap<TCHAR, TCHAR>& SqadDelims, int32* ResultIndex, TSharedPtr<EMStringParseNode> ParentParserNode, int32 Start, const TCHAR Squad)
{
	int32 SquadStart = Start;

	TSharedPtr<EMStringParseNode> CurrentNode = CreateParseNode(ParentParserNode);
	CurrentNode->Start = Start;
	CurrentNode->End = ParseString.Len();

	for (int32 i = Start; i < ParseString.Len(); i++)
	{
		const TCHAR& ch = ParseString[i];

		if (0 != Squad && ch == Squad)
		{
			*ResultIndex = i - Start;

			if (1 <= i - SquadStart)
			{
				TSharedPtr<EMStringParseNode> ParserNode = CreateParseNode(CurrentNode);
				ParserNode->Start = SquadStart;
				ParserNode->End = i;
				CurrentNode->End = i;

				return CurrentNode;
			}
			else {
				return nullptr;
			}
		}
		else
		{
			const TCHAR* ItDelim = SqadDelims.Find(ch);
			if (nullptr != ItDelim)
			{
				if (0 != *ItDelim)
				{
					int32 SquadResultIndex = INDEX_NONE;
					if (SquadStart < i)
					{
						TSharedPtr<EMStringParseNode> ParserNode = CreateParseNode(CurrentNode);
						ParserNode->Start = SquadStart;
						ParserNode->End = i;
					}

					ParsingSquad(ParseString, SqadDelims, &SquadResultIndex, CurrentNode, i + 1, *ItDelim);
					if (INDEX_NONE == SquadResultIndex)
					{
						return nullptr;
					}
					i += SquadResultIndex + 1;

					SquadStart = i + 1;
				}
			}
		}
	}

	int32 StrLength = FCString::Strlen(*ParseString);
	if (StrLength - 1 >= SquadStart)
	{
		TSharedPtr<EMStringParseNode> ParserNode = CreateParseNode(CurrentNode);
		ParserNode->Start = SquadStart;
		ParserNode->End = StrLength;
	}
	return (0 != Squad) ? nullptr : CurrentNode;
}

bool EMStringParser::DelimContains(const FString& Delim, const FString& ParseString, int32 ParseStringIndex)
{
	if (Delim.Len() > (ParseString.Len() - 1) - ParseStringIndex)
	{
		return false;
	}
	for (int32 i = 0; i < Delim.Len(); i++)
	{
		if ((*Delim)[i] != (*ParseString)[ParseStringIndex + i])
		{
			return false;
		}
	}
	return true;
}

TSharedPtr<EMStringParseNode> EMStringParser::ParsingDelims(
	const FString& ParseString, const TArray<FString>& Delims, const TArray<FString>& CommentDelims, int32* ResultIndex, TSharedPtr<EMStringParseNode> ParentParserNode)
{
	int32 SquadStart = 0;
	TSharedPtr<EMStringParseNode> CurrentNode = CreateParseNode(ParentParserNode);
	CurrentNode->Start = SquadStart;
	CurrentNode->End = ParseString.Len();

	bool IsComment = false;

	for (int32 i = 0; i < ParseString.Len(); i++)
	{
		if(2 == CommentDelims.Num())
		{
			if (true == DelimContains(CommentDelims[0], ParseString, i))
			{
				SquadStart = i;
				IsComment = true;
				i += CommentDelims[0].Len() - 1;
				continue;
			}
			if (true == DelimContains(CommentDelims[1], ParseString, i))
			{
				if(i > SquadStart + CommentDelims[0].Len())
				{
					TSharedPtr<EMStringParseNode> ParserNode = CreateParseNode(CurrentNode);

					ParserNode->Start = SquadStart;
					ParserNode->End = i + CommentDelims[1].Len();
				}
				IsComment = false;
				i += CommentDelims[1].Len() - 1;
				SquadStart = i + CommentDelims[1].Len();
				continue;
			}
		}
		if (true == IsComment)
		{
			continue;
		}

		for(int32 j=0; j < Delims.Num(); j++)
		{
			if(true == DelimContains(Delims[j], ParseString, i))
			{
				if (SquadStart < i)
				{
					TSharedPtr<EMStringParseNode> ParserNode = CreateParseNode(CurrentNode);
					ParserNode->Start = SquadStart;
					ParserNode->End = i;
				}			
				TSharedPtr<EMStringParseNode> DelimNode = CreateParseNode(CurrentNode);
				DelimNode->Start = i;
				DelimNode->End = i + Delims[j].Len();

				SquadStart = i + Delims[j].Len();

				i+= Delims[j].Len() - 1;
				break;
			}
		}
	}

	int32 StrLength = FCString::Strlen(*ParseString);
	if (StrLength - 1 >= SquadStart)
	{
		TSharedPtr<EMStringParseNode> ParserNode = CreateParseNode(CurrentNode);
		ParserNode->Start = SquadStart;
		ParserNode->End = StrLength;
	}
	return CurrentNode;
}

bool EMStringParser::IsFoundDelims(const TArray<FString>& Delims, const TCHAR* String)
{
	for (int32 i = 0; i < Delims.Num(); i++)
	{
		const TCHAR* FindAddr = FCString::Strfind(String, *Delims[i]);
		if (nullptr != FindAddr && FindAddr == String)
		{
			return true;
		}
	}
	return false;
}

bool EMStringParser::IsFoundDelims(const TCHAR Operator, const FString& String)
{
	if ((*String)[0] == Operator)
	{
		return true;
	}
	return false;
}

bool EMStringParser::IsOperator(const TCHAR ch)
{
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
	{
		return true;
	}
	return false;
}

bool EMStringParser::IsNaturalNumber(const FString& String)
{
	for(int32 i=0; i < String.Len(); i++)
	{
		if((*String)[i] < 0 || (*String)[i] > 9)
		{
			return false;
		}
	}
	return true;
}

bool EMStringParser::IsSpace(const FString& String)
{
	if (true == String.Equals(TEXT(" ")) ||
		true == String.Equals(TEXT("\t")))
	{
		return true;
	}
	return false;
}

bool EMStringParser::IsLF(const FString& String)
{
	if (true == String.Equals(TEXT("\r")) || true == String.Equals(TEXT("\r\n")) ||
		true == String.Equals(TEXT("\n")))
	{
		return true;
	}
	return false;
}

bool EMStringParser::IsSpaceAndLF(const FString& String)
{
	if (true == IsSpace(String) || true == IsLF(String))
	{
		return true;
	}
	return false;
}

bool EMStringParser::CalcOperation(TArray<FString>& InStrings, const FString& FoundDelims)
{
	for(int32 OperationIndex = 0; OperationIndex < InStrings.Num(); OperationIndex++)
	{
		if (1 > InStrings[OperationIndex].Len())
		{
			continue;
		}

		if(1 > OperationIndex || OperationIndex >= InStrings.Num() - 1)
		{
			continue;
		}
		for(int32 j=0; j < FoundDelims.Len(); j++)
		{
			if (false == IsFoundDelims((*FoundDelims)[j], InStrings[OperationIndex]))
			{
				continue;
			}
			if (false == InStrings[OperationIndex - 1].IsNumeric() || false == InStrings[OperationIndex + 1].IsNumeric())
			{
				break;
			}
			int64 ResultValue = 0;
			if (InStrings[OperationIndex].Equals(TEXT("/")))
			{				
				int64 B = FCString::Atoi64(*InStrings[OperationIndex + 1]);
				if(B == 0) ResultValue = 0;
				else {
					int64 A = FCString::Atoi64(*InStrings[OperationIndex - 1]);
					ResultValue = A / B;
				}
			}
			else if (InStrings[OperationIndex].Equals(TEXT("*")))
			{
				ResultValue = FCString::Atoi64(*InStrings[OperationIndex - 1]) * FCString::Atoi64(*InStrings[OperationIndex + 1]);
			}
			else if (InStrings[OperationIndex].Equals(TEXT("+")))
			{
				ResultValue = FCString::Atoi64(*InStrings[OperationIndex - 1]) + FCString::Atoi64(*InStrings[OperationIndex + 1]);
			}
			else if (InStrings[OperationIndex].Equals(TEXT("-")))
			{
				ResultValue = FCString::Atoi64(*InStrings[OperationIndex - 1]) - FCString::Atoi64(*InStrings[OperationIndex + 1]);
			}
			InStrings[OperationIndex - 1] = FString::Printf(TEXT("%d"), ResultValue);

			InStrings.RemoveAt(OperationIndex);
			InStrings.RemoveAt(OperationIndex);

			OperationIndex--;
			break;
		}
	}
	return true;
}

void EMStringParser::ArrangeSign(TArray<FString>& InStrings)
{
	for (int32 i = 0; i < InStrings.Num(); i++)
	{
		TCHAR Ch = (*InStrings[i])[0];
		if (1 != InStrings[i].Len() || i + 1 >= InStrings.Num())
		{
			continue;
		}
		bool IsRemoveAt = false;
		if (Ch == '-')
		{
			TCHAR NextCh = (*InStrings[i + 1])[0];
			if (NextCh >= '0' && NextCh <= '9')
			{
				InStrings[i] += InStrings[i + 1];
				IsRemoveAt = true;
			}
			else if (NextCh == '-')
			{
				InStrings[i] = TEXT("+");
				IsRemoveAt = true;
			}
			else if (NextCh == '+')
			{
				InStrings.RemoveAt(i + 1);
				IsRemoveAt = true;
			}
		}
		else if (Ch == '+')
		{
			TCHAR NextCh = (*InStrings[i + 1])[0];
			if (NextCh >= '0' && NextCh <= '9')
			{
				InStrings[i] += InStrings[i + 1];
				IsRemoveAt = true;
			}
			else if (NextCh == '-')
			{
				InStrings[i] = TEXT("-");
				IsRemoveAt = true;
			}
			else if (NextCh == '+')
			{
				IsRemoveAt = true;
			}
		}
		if (true == IsRemoveAt)
		{
			InStrings.RemoveAt(i + 1);
			i--;
		}
	}
}

TArray<FString> EMStringParser::CalcStringArray(TArray<FString>& InStrings)
{
	ArrangeSign(InStrings);

	CalcOperation(InStrings, TEXT("*/"));

	int64 ResultValue = 0;
	TCHAR HeadOperator = 0;
	TCHAR TailOperator = 0;
	bool IsValue = false;
	for (int32 i = 0; i < InStrings.Num(); i++)
	{
		if (true == IsOperator((*InStrings[i])[0]) && InStrings[i].Len() == 1)
		{
			if(i == 0)
			{
				HeadOperator = (*InStrings[i])[0];
			}
			else if(i == InStrings.Num() - 1)
			{
				TailOperator = (*InStrings[i])[0];
			}
			continue;
		}
		ResultValue += FCString::Atoi64(*InStrings[i]);
		IsValue = true;
	}

	TArray<FString> ResultStrings;

	if (0 != HeadOperator) ResultStrings.Emplace(FString::Printf(TEXT("%c"), HeadOperator));
	if(true == IsValue)
	{
		ResultStrings.Emplace(FString::Printf(TEXT("%d"), ResultValue));
	}
	if(0 != TailOperator) ResultStrings.Emplace(FString::Printf(TEXT("%c"), TailOperator));

	return ResultStrings;
}

TArray<FString> EMStringParser::GetCalcStringSquad(const FString& InString, TSharedPtr<EMStringParseNode> ParseRootnode)
{	
	if (ParseRootnode->ChildNodes.Num() == 0)
	{
		int32 Length = ParseRootnode->End - ParseRootnode->Start;
		FString LeafString;
		LeafString.Append(&(*InString)[ParseRootnode->Start], Length);

		TSharedPtr<EMStringParseNode> LeafNode = ParsingDelims(LeafString, OperationDelims, TArray<FString>());

		TArray<FString> OutStrings;
		CollectedString(LeafString, LeafNode, OutStrings);

		return CalcStringArray(OutStrings);
	}
	TArray<FString> ResultStrings;
	for (int32 i = 0; i < ParseRootnode->ChildNodes.Num(); i++)
	{
		ResultStrings += GetCalcStringSquad(InString, ParseRootnode->ChildNodes[i]);
	}
	ResultStrings = CalcStringArray(ResultStrings);
	return ResultStrings;
}

int64 EMStringParser::Calculator(const FString& InString)
{
	TMap<TCHAR, TCHAR> SqadDelims;
	SqadDelims.Emplace('(', ')');

	FString NoSpaceString = InString.Replace(TEXT(" "), TEXT(""));
	NoSpaceString = NoSpaceString.Replace(TEXT("\t"), TEXT(""));

	TSharedPtr<EMStringParseNode> ParseRootnode = ParsingSquad(NoSpaceString, SqadDelims);
	if (nullptr == ParseRootnode)
	{
		return 0;
	}
	TArray<FString> SquadResults = GetCalcStringSquad(NoSpaceString, ParseRootnode);

	ArrangeSign(SquadResults);

	TArray<FString> FinalResults = CalcStringArray(SquadResults);

	int64 ResultValue = 0;
	for (int32 i = 0; i < FinalResults.Num(); i++)
	{
		ResultValue += FCString::Atoi64(*FinalResults[i]);
	}
	return ResultValue;
}
