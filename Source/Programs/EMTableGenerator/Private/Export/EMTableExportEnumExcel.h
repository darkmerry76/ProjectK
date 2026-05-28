#pragma once

#include "EMTableGenerator.h"
#include "EMTableExport.h"
#include "Parser/EMCppParser.h"
#include "libxl.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableExportEnumExcel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableExportEnumExcel : public EMTableExport
{
public:
	EMTableExportEnumExcel();

protected:

	bool DoExport(libxl::Book* EnumBook, const TCHAR* SheetName, const eEnumOnwerType OwnerType);

	virtual bool DoExport() override;
};
