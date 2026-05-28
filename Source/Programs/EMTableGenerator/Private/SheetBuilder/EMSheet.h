#pragma once

#include "EMTableGenerator.h"
#include "Schema/EMSchema.h"
#include "EMStructure.h"
#include "libxl.h"
#include "pugixml.hpp"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

#define TEST_ROW_COUNT_MULTIPLY 1

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheet
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheet
{
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// EMSheetRow
	////////////////////////////////////////////////////////////////////////////////////////////////////
	struct EMSheetRow
	{
		TArray<FString> Columns;
		TMap<FString,int32> ColumnsMap;

		void CreateColumnMaps();
		int32 FindColumn(const FString& Name) const;
	};

public:
	EMSheet();
	virtual ~EMSheet();

	const FString& GetCell(int32 Column, int32 Row) const;
	
	void SetExcelName(const FString& ExecelName);
	const FString GetExcelName() const;

	void SetSheetName(const FString& ExecelName);
	const FString GetSheetName() const;

	const TArray<TSharedPtr<EMSheetRow>>& GetRow() const { return Rows; }

	int32 GetLastColumn() const{ return LastCol; };
	int32 GetLastRow() const { return Rows.Num(); };

	void SetColumnRemap(int32 OldColumn, int32 NewColumn);

	void Build();

protected:

	virtual void DoBuild() { };

	void InitColumnRemaps();

	bool CheckReadOrNot(int Row, int Column, FString ColumnString, int32& ReadOrNotCol, bool& IsIgnoreRow);

protected:
	TArray<int32> ColumnRemaps;
	TArray<TSharedPtr<EMSheetRow>> Rows;
	int32 LastCol;

	FString ExcelName;
	FString SheetName;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetXML
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheetXML : public EMSheet
{
public:
	EMSheetXML(pugi::xml_node InXMLNode);
	virtual ~EMSheetXML();

	virtual void DoBuild() override;

protected:
	void RecursiveXMLNode(pugi::xml_node Node, FString& Out);
	void XMLNodeParsing(FString& NodeString, pugi::xml_node Node);

protected:
	pugi::xml_node XMLNode;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetLibXL
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheetLibXL : public EMSheet
{
public:
	EMSheetLibXL(libxl::Sheet* InLibXLSheet);
	virtual ~EMSheetLibXL();

protected:
	virtual void DoBuild();

protected:
	libxl::Sheet* LibXLSheet;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetJSON
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheetJSON : public EMSheet
{
protected:
	struct FEMSheetHeaderRow
	{
		FEMSheetHeaderRow();

		void Init();
		int32 FindVariableToIndex(const FName& VariableName) const;

		TSharedPtr<EMSheetRow>		TypeRow;
		TSharedPtr<EMSheetRow>		DescriptionRow;
		TSharedPtr<EMSheetRow>		VariableRow;

	protected:
		TMap<FName, int32> VariableNameToIndexMap;
	};

public:
	EMSheetJSON(const FName& InSchemaName, TSharedPtr<FJsonValue> InSheetJsonValue);
	virtual ~EMSheetJSON();

protected:
	virtual void DoBuild();

	TSharedPtr<EMSheet::EMSheetRow> CollectRowData(TSharedPtr<FEMStructure> TypeStructure, const FEMSheetHeaderRow& HeaderRow, const TSharedPtr<FJsonObject> JsonObject);

	void BuildType(FEMSheetHeaderRow& OutHeaderRow, const TSharedPtr<FEMStructure> ParentStructure, const TSharedPtr<FEMStructure> TypeStructure);

protected:
	FName SchemaName;
	TSharedPtr<FJsonValue> SheetJsonValue;
};