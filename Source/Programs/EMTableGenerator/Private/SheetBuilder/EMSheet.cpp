#include "EMSheet.h"

#include "CondensedJsonPrintPolicy.h"
#include "EMSheetBuilder.h"
#include "Schema/EMSchemaMgr.h"
#include "Config/EMTableConfig.h"

#include "Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "Core/Public/HAL/PlatformFilemanager.h"
#include "Core/Public/Misc/Paths.h"
#include "RunTime/Launch//Public/LaunchEngineLoop.h"
#include "Core/Public/Modules/ModuleManager.h"

#include "JsonUtilities/Public/JsonObjectConverter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheet::EMSheetRow
////////////////////////////////////////////////////////////////////////////////////////////////////

void EMSheet::EMSheetRow::CreateColumnMaps()
{
	for (int32 i = 0; i < Columns.Num(); ++i)
	{
		ColumnsMap.Emplace(Columns[i], i);
	}
}

int32 EMSheet::EMSheetRow::FindColumn(const FString& Name) const
{
	const int32* Result = ColumnsMap.Find(Name);
	if (nullptr != Result)
	{
		return *Result;
	}
	return INDEX_NONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheet
////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheet::EMSheet()
{
}

EMSheet::~EMSheet()
{

}

void EMSheet::SetExcelName(const FString& InExcelName)
{
	ExcelName = InExcelName;
}

const FString EMSheet::GetExcelName() const
{
	return ExcelName;
}

void EMSheet::SetSheetName(const FString& InSheetName)
{
	SheetName = InSheetName;
}

const FString EMSheet::GetSheetName() const
{
	return SheetName;
}

const FString& EMSheet::GetCell(int32 Column, int32 Row) const
{
	//Column = ColumnRemaps[Column];

	if (Row >= Rows.Num() || Column >= Rows[Row]->Columns.Num() || INDEX_NONE == Column)
	{
		static FString NullString(TEXT(""));
		return NullString;
	}

	return Rows[Row]->Columns[Column];
}

void EMSheet::InitColumnRemaps()
{
	ColumnRemaps.Empty();

	for(int32 i=0; i < LastCol; i++)
	{
		ColumnRemaps.Emplace(i);
	}
}

void EMSheet::SetColumnRemap(int32 OldColumn, int32 NewColumn)
{
	int32 NewRemapSize = (NewColumn - ColumnRemaps.Num() ) + 1;

	if(0 < NewRemapSize)
	{
		ColumnRemaps.AddZeroed(NewRemapSize);
		LastCol = NewColumn;
	}

	ColumnRemaps[OldColumn] = INDEX_NONE;
	ColumnRemaps[NewColumn] = OldColumn;	
}

void EMSheet::Build()
{
	DoBuild();

	InitColumnRemaps();
}

bool EMSheet::CheckReadOrNot(int Row, int Column, FString ColumnString, int32& ReadOrNotCol, bool& IsIgnoreRow)
{
	// 변수명에 ReadOrNot이 있는지 확인 해준다. [Row (2) 변수명 정의]
	if (Row == 2)
	{
		if (ReadOrNotCol == -1)
		{
			if (ColumnString.Equals(TEXT("ReadOrNot"), ESearchCase::IgnoreCase))
			{
				ReadOrNotCol = Column;
			}
		}
	}
	if (Row > 2)
	{
		if (ReadOrNotCol == Column)
		{
			if (ColumnString.Equals(TEXT("0")))
			{
				IsIgnoreRow = true;
				return false;
			}
		}
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetXML
////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetXML::EMSheetXML(pugi::xml_node InXMLNode) : XMLNode(InXMLNode)
{
}

EMSheetXML::~EMSheetXML()
{

}

enum eXMLCellType
{
	eXMLCellType_Number = 0,
	eXMLCellType_String,
};

void EMSheetXML::RecursiveXMLNode(pugi::xml_node Node, FString& Out)
{
	for (auto Child : Node)
	{
		const TCHAR* val = Child.value();
		if (val)
		{
			eXMLCellType CellType = eXMLCellType::eXMLCellType_Number;

			pugi::xml_attribute TypeAttribute = Node.attribute(TEXT("Type"));
			if (nullptr == TypeAttribute)
			{
				TypeAttribute = Node.attribute(TEXT("ss:Type"));
				
			}
			if(nullptr != TypeAttribute)
			{
				if (0 == _tcsicmp(TypeAttribute.as_string(), TEXT("string")))
				{
					CellType = eXMLCellType_String;
				}
			}

			bool IsSpecialDelim = false;
			for(int32 i=0; i < FCString::Strlen(val); i++)
			{
				if(CellType != eXMLCellType_String)
				{
					if (val[i] == '#' || val[i] == '*' || val[i] == '$')
					{
						IsSpecialDelim = true;
					}
				}
			}
			if(false == IsSpecialDelim)
				Out += val;
		}
		RecursiveXMLNode(Child, Out);
	}
}

void EMSheetXML::XMLNodeParsing(FString& NodeString, pugi::xml_node Node)
{

}

void EMSheetXML::DoBuild()
{
	LastCol = 0;
	int32 LocalLastRow = 0;
	int ReadOrNotCol = -1;

	for (pugi::xml_node pRow : XMLNode)
	{
		if (0 != FCString::Stricmp(pRow.name(), TEXT("Row")) &&
			0 != FCString::Stricmp(pRow.name(), TEXT("ss:Row"))) 
			continue;

		pugi::xml_attribute str_Row_attr = pRow.attribute(TEXT("Index"));
		if (nullptr == str_Row_attr)
		{
			str_Row_attr = pRow.attribute(TEXT("ss:Index"));
		}

		pugi::xml_attribute str_row_hidden = pRow.attribute(TEXT("Hidden"));
		if (nullptr == str_row_hidden)
		{
			str_row_hidden = pRow.attribute(TEXT("ss:Hidden"));
		}
		const pugi::char_t* str_hidden_row = str_row_hidden.as_string();
		if (str_hidden_row != NULL && 0 == FCString::Stricmp(str_hidden_row, TEXT("1")))
		{
			continue;
		}

		if (nullptr != str_Row_attr)
		{
			int32 nextRow = str_Row_attr.as_int();

			for (int32 i = LocalLastRow; i < nextRow - 1; i++)
			{
				Rows.Emplace(MakeShareable(new EMSheetRow));
				LocalLastRow++;
			}
		}

		TSharedPtr<EMSheetRow> NewHeadRow = MakeShareable(new EMSheetRow);
		bool IsIgnoreRow = false;

		int32 currentColumn = 0;
		for (pugi::xml_node pCell : pRow)
		{
			if (0 != FCString::Stricmp(pCell.name(), TEXT("Cell")) &&
				0 != FCString::Stricmp(pCell.name(), TEXT("ss:Cell")))
				continue;

			pugi::xml_attribute str_col_attr = pCell.attribute(TEXT("Index"));
			if (nullptr == str_col_attr)
			{
				str_col_attr = pCell.attribute(TEXT("ss:Index"));
			}

			currentColumn++;
			EMTableGrammar::CurrentColumn = currentColumn;
			int32 nextColumn = currentColumn;

			if (nullptr != str_col_attr)
			{
				nextColumn = str_col_attr.as_int();
			}
			FString CellString;
			RecursiveXMLNode(pCell.child(TEXT("Data")), CellString);
			RecursiveXMLNode(pCell.child(TEXT("ss:Data")), CellString);

			for (int32 Column = currentColumn; Column <= nextColumn; Column++)
			{
				FString ColumnString = Column == nextColumn ? CellString : TEXT("");
				if (false == CheckReadOrNot(LocalLastRow, Column, ColumnString, ReadOrNotCol, IsIgnoreRow))
				{
					break;
				}

				ColumnString = ColumnString.TrimStart();
				NewHeadRow->Columns.Emplace(ColumnString);
			}
			currentColumn = nextColumn;
		}
		if(LastCol < currentColumn) LastCol = currentColumn;

		LocalLastRow++;

		if (IsIgnoreRow == false)
		{
			Rows.Emplace(NewHeadRow);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetLibXL
////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetLibXL::EMSheetLibXL(libxl::Sheet* InLibXLSheet) : LibXLSheet(InLibXLSheet)
{
}

EMSheetLibXL::~EMSheetLibXL()
{
}

void EMSheetLibXL::DoBuild()
{
	LastCol = LibXLSheet->lastCol();

	int ReadOrNotCol = -1;

	for (int32 Row = LibXLSheet->firstRow(); Row < LibXLSheet->lastRow(); Row++)
	{
		if (true == LibXLSheet->rowHidden(Row))
		{
			continue;
		}
		TSharedPtr<EMSheetRow> NewHeadRow = MakeShareable(new EMSheetRow);
		bool IsIgnoreRow = false;

		for (int32 Column = LibXLSheet->firstCol(); Column < LibXLSheet->lastCol(); Column++)
		{
			const TCHAR* TempChar = LibXLSheet->readStr(Row, Column);

			FString ColumnString((nullptr != TempChar) ? TempChar : TEXT(""));

			if (false == CheckReadOrNot(Row, Column, ColumnString, ReadOrNotCol, IsIgnoreRow))
			{
				break;
			}

			ColumnString = ColumnString.TrimStart();
			NewHeadRow->Columns.Emplace(ColumnString);
		}

		if(IsIgnoreRow == false)
		{
			Rows.Emplace(NewHeadRow);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetJSON::FEMSheetHeaderRow
////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetJSON::FEMSheetHeaderRow::FEMSheetHeaderRow()
{
	TypeRow			= MakeShareable(new EMSheetRow());
	DescriptionRow	= MakeShareable(new EMSheetRow());
	VariableRow		= MakeShareable(new EMSheetRow());
}

void EMSheetJSON::FEMSheetHeaderRow::Init()
{
	VariableNameToIndexMap.Empty();

	for (int32 i = 0; i < VariableRow->Columns.Num(); ++i)
	{
		TArray<FString> OutStrings;
		VariableRow->Columns[i].ParseIntoArray(OutStrings, TEXT("#"));
		ensure(0 != OutStrings.Num());
		if (0 != OutStrings.Num())
		{
			if (1 < OutStrings.Num())
			{
				TArray<FString> ClassStrings;
				OutStrings[1].ParseIntoArray(ClassStrings, TEXT("."));

				FString FinalName = OutStrings[0];

				for (int32 j = 1; j < ClassStrings.Num(); ++j)
				{
					FinalName += TEXT(".");
					FinalName += ClassStrings[j];
				}
				VariableNameToIndexMap.Emplace(*FinalName, i);
			}
			else
			{
				VariableNameToIndexMap.Emplace(*OutStrings[0], i);
			}
		}
	}
}

int32 EMSheetJSON::FEMSheetHeaderRow::FindVariableToIndex(const FName& VariableName) const
{
	const int32* Result = VariableNameToIndexMap.Find(VariableName);
	if (nullptr == Result)
	{
			return INDEX_NONE;
	}
	return *Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetJSON
////////////////////////////////////////////////////////////////////////////////////////////////////
EMSheetJSON::EMSheetJSON(const FName& InSchemaName, TSharedPtr<FJsonValue> InSheetJsonValue) : SchemaName(InSchemaName)
	, SheetJsonValue(InSheetJsonValue)
{
}

EMSheetJSON::~EMSheetJSON()
{
}

void EMSheetJSON::BuildType(FEMSheetHeaderRow& OutHeaderRow, const TSharedPtr<FEMStructure> ParentStructure, const TSharedPtr<FEMStructure> TypeStructure)
{
	if (false == TypeStructure.IsValid())
	{
		return;
	}
	for (auto MemberItr : TypeStructure->GetMembers())
	{
		OutHeaderRow.DescriptionRow->Columns.Emplace(TEXT(""));

		FString OutType;
		FString OutVariable;
		bool IsKey = ParentStructure->IsKeyUnique(MemberItr.Key);
		if (true == IsKey)
		{
			OutType += TEXT("(");
		}
		OutVariable += MemberItr.Key.ToString();
		const TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(MemberItr.Value->GetType());
		if (false == MemberStructure.IsValid())
		{
			OutType += MemberItr.Value->GetType().ToString();
		}
		else
		{
			OutType += FString(TEXT("struct#")) + MemberStructure->GetName().ToString();	
		}
		
		OutType += true == MemberItr.Value->IsArray() ? TEXT("[]") : TEXT("");

		if (true == IsKey)
		{
			OutType += TEXT(")");
		}
		OutHeaderRow.TypeRow->Columns.Emplace(OutType);

		OutHeaderRow.VariableRow->Columns.Emplace(OutVariable);
	}
}

void EMSheetJSON::DoBuild()
{
	LastCol = 0;

	ensure(nullptr != SheetJsonValue);
	if (nullptr == SheetJsonValue)
	{
		return;
	}

	TSharedPtr<FEMStructure> TypeStructure;

	const TSharedPtr<FEMStructure> Structure = FEMStructureManager::Inst().GetStructure(*FString::Printf(TEXT("%s::schema"), *SchemaName.ToString()));
	ensure(true == Structure.IsValid());
	if (false == Structure.IsValid())
	{
		return;
	}
	if (SheetJsonValue->Type == EJson::Object)
	{
		TypeStructure = Structure;
	}
	else if(SheetJsonValue->Type == EJson::Array)
	{
		const TSharedPtr<FEMStructureMember> StructureMember = Structure->GetMember(*GetSheetName());
		ensure(true == StructureMember.IsValid());
		if (false == StructureMember.IsValid())
		{
			return;
		}

		TypeStructure = FEMStructureManager::Inst().GetStructure(StructureMember->GetType());
	}
	ensure(true == TypeStructure.IsValid());

	FEMSheetHeaderRow HeaderRow;

	BuildType(HeaderRow, Structure, TypeStructure);
	HeaderRow.Init();

	Rows.Emplace(HeaderRow.DescriptionRow);
	Rows.Emplace(HeaderRow.TypeRow);
	Rows.Emplace(HeaderRow.VariableRow);

	LastCol = HeaderRow.TypeRow->Columns.Num();

	if(SheetJsonValue->Type == EJson::Array)
	{
		const TArray<TSharedPtr<FJsonValue>>& ChildArrayFields = SheetJsonValue->AsArray();

		for(auto ChildIt : ChildArrayFields)
		{
			TSharedPtr<EMSheetRow> NewSheetRow = CollectRowData(TypeStructure, HeaderRow, ChildIt->AsObject());
			if (true == NewSheetRow.IsValid())
			{
				Rows.Emplace(NewSheetRow);
			}
		}
	}
	else if (SheetJsonValue->Type == EJson::Object)
	{

		TSharedPtr<EMSheetRow> NewSheetRow = CollectRowData(TypeStructure, HeaderRow, SheetJsonValue->AsObject());
		if(true == NewSheetRow.IsValid())
		{
			Rows.Emplace(NewSheetRow);
		}
	}
	else
	{
		ensure(0);
	}
}

TSharedPtr<EMSheet::EMSheetRow> EMSheetJSON::CollectRowData(TSharedPtr<FEMStructure> TypeStructure, const FEMSheetHeaderRow& HeaderRow, const TSharedPtr<FJsonObject> JsonObject)
{
	TSharedPtr<EMSheet::EMSheetRow> NewSheetRow = MakeShareable(new EMSheetRow);
	NewSheetRow->Columns.SetNum(HeaderRow.TypeRow->Columns.Num());

	if (false == JsonObject.IsValid())
	{
		return nullptr;
	}

	for (int32 i = 0; i < TypeStructure->GetMemberCount(); ++i)
	{
		TSharedPtr<FEMStructureMember> Member = TypeStructure->GetMember(i);
		if (false == Member.IsValid())
		{
			continue;
		}
		if (true == JsonObject->HasField(Member->GetName().ToString()))
		{
			TSharedPtr<FJsonValue> JsonValue = JsonObject->TryGetField(Member->GetName().ToString());

			FString OutputString;

			const TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(*Member->GetType().ToString());
			TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString, 0);

			if (JsonValue->Type == EJson::Object)
			{
				ensure(true == MemberStructure.IsValid());
				if(MemberStructure.IsValid())
				{
					FJsonSerializer::Serialize(JsonValue->AsObject().ToSharedRef(), Writer);
				}
			}
			else if (JsonValue->Type == EJson::Array)
			{
				if (MemberStructure.IsValid())
				{
					FJsonSerializer::Serialize(JsonValue->AsArray(), Writer);
				}
				else
				{
					const TArray<TSharedPtr<FJsonValue>>& JsonArray = JsonValue->AsArray();
					for(int32 j=0; j < JsonArray.Num(); ++j)
					{
						OutputString += JsonArray[j]->AsString() + (j < JsonArray.Num() - 1 ? TEXT("|") : TEXT(""));
					}
				}
			}
			else if (
				JsonValue->Type == EJson::String ||
				JsonValue->Type == EJson::Number ||
				JsonValue->Type == EJson::Boolean)
			{
				if(Member->GetTableVariableType() == EMTableCore::EEMVariabaleType::Enum)
				{
					OutputString = JsonValue->AsString();
				}
				else
				{
					OutputString = JsonValue->AsString();
				}
			}
			else
			{
				OutputString = JsonValue->AsString();
			}
			NewSheetRow->Columns[i] = OutputString;
		}
	}
	return NewSheetRow;
}