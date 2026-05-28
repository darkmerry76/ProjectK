#include "EMTableExportEnumExcel.h"
#include "Config/EMTableConfig.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "Utility/EMTableUtility.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableExportEnumExcel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableExportEnumExcel::EMTableExportEnumExcel() : EMTableExport()
{
}

bool EMTableExportEnumExcel::DoExport(libxl::Book* EnumBook, const TCHAR* SheetName, const eEnumOnwerType OwnerType)
{
	libxl::Sheet* EnumSheet = EnumBook->addSheet(SheetName);
	if (nullptr == EnumSheet)
	{
		return false;
	}

	libxl::Format* EnumTitleFormat = EnumBook->addFormat();

	EnumTitleFormat->setAlignH(libxl::ALIGNH_LEFT);
	EnumTitleFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
	EnumTitleFormat->setBorder(libxl::BORDERSTYLE_THIN);
	EnumTitleFormat->setPatternForegroundColor(libxl::COLOR_LIGHTTURQUOISE);

	libxl::Format* EnumValueNameFormat = EnumBook->addFormat();
	EnumValueNameFormat->setAlignH(libxl::ALIGNH_LEFT);
	EnumValueNameFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
	EnumValueNameFormat->setBorder(libxl::BORDERSTYLE_THIN);
	EnumValueNameFormat->setPatternForegroundColor(libxl::COLOR_LIGHTYELLOW);

	libxl::Format* EnumNameFormat = EnumBook->addFormat();
	EnumNameFormat->setAlignH(libxl::ALIGNH_LEFT);
	EnumNameFormat->setBorder(libxl::BORDERSTYLE_THIN);

	libxl::Format* EnumValueFormat = EnumBook->addFormat();
	EnumValueFormat->setAlignH(libxl::ALIGNH_RIGHT);
	EnumValueFormat->setBorder(libxl::BORDERSTYLE_THIN);

	const int32 MAX_COLUMN_INPUT = 4;

	int32 RowCounts[MAX_COLUMN_INPUT];
	FMemory::Memzero(RowCounts, sizeof(RowCounts));

	for (auto it = EMCppParser::Get().GetEnums().CreateConstIterator(); it; ++it)
	{
		if (false == it->Value.IsValid())
		{
			continue;
		}

		const TSharedPtr<EMCppEnumNode> EnumNode = it->Value;
		if (false == EnumNode.IsValid() || EnumNode->OwnerType != OwnerType)
		{
			continue;
		}

		int32 MinRowCount = 999999;
		int32 SelectColumn = 0;

		for (int32 Col = 0; Col < MAX_COLUMN_INPUT; Col++)
		{
			if (MinRowCount > RowCounts[Col])
			{
				MinRowCount = RowCounts[Col];
				SelectColumn = Col;
			}
		}

		int32 RowCount = RowCounts[SelectColumn] + 1;
		int32 ColumnCount = (SelectColumn * 5);

		EnumSheet->writeStr(RowCount, ColumnCount + 1, *FString::Printf(TEXT("%s"), *EnumNode->Name), EnumTitleFormat);

		RowCount++;
		EnumSheet->writeStr(RowCount, ColumnCount + 1, *EMCppParser::Get().GetStringNodeName(EnumNode.Get()), EnumValueNameFormat);
		EnumSheet->writeStr(RowCount, ColumnCount + 2, *FString::Printf(TEXT("Value")), EnumValueNameFormat);
		EnumSheet->writeStr(RowCount, ColumnCount + 3, *FString::Printf(TEXT("Name")), EnumValueNameFormat);
		EnumSheet->writeStr(RowCount, ColumnCount + 4, *FString::Printf(TEXT("Desc")), EnumValueNameFormat);
		RowCount++;

		for (int32 i = 0; i < EnumNode->Members.Num(); i++)
		{
			const TSharedPtr<EMCppEnumMemberNode> EnumMemberNode = EnumNode->Members[i];

			if (false == EnumMemberNode.IsValid())
			{
				continue;
			}

			EnumSheet->writeStr(RowCount, ColumnCount + 1, *FString::Printf(TEXT("%s"), &(*EnumMemberNode->Name)[EnumNode->Prefix.Len()]), EnumValueNameFormat);
			EnumSheet->writeStr(RowCount, ColumnCount + 2, *FString::Printf(TEXT("%d"), EnumMemberNode->Value), EnumValueFormat);
			EnumSheet->writeStr(RowCount, ColumnCount + 3, *FString::Printf(TEXT("%s"), *EnumMemberNode->Name), EnumNameFormat);
			EnumSheet->writeStr(RowCount, ColumnCount + 4, *FString::Printf(TEXT("%s"), *EnumMemberNode->Description), EnumNameFormat);

			RowCount++;
		}
		RowCounts[SelectColumn] = RowCount;
	}
	for (int32 Col = 0; Col < MAX_COLUMN_INPUT; Col++)
	{
		EnumSheet->setAutoFitArea(0, (Col * 5) + 1, -1, (Col * 5) + 3);
	}
	return true;
}

bool EMTableExportEnumExcel::DoExport()
{
	if (1 > EMCppParser::Get().GetEnums().Num())
	{
		return false;
	}

	libxl::Book* EnumBook = xlCreateXMLBook();
	EnumBook->setKey(TEXT("daesung kim"), TEXT("windows-222b250607c5e40769bf6962aevfj6u6"));
	if (nullptr == EnumBook)
	{
		return false;
	}
	DoExport(EnumBook, TEXT("Server"), eEnumOnwerType::eEnumOnwerType_Cpp);
	DoExport(EnumBook, TEXT("Client"), eEnumOnwerType::eEnumOnwerType_Excel);

	EnumBook->save(*EMTableConfig::Get().GetCodeSection().EnumExcelFile);
	EnumBook->release();
	return true;
}
