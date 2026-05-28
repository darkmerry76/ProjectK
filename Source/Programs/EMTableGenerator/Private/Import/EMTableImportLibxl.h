#pragma once

#include "EMTableGenerator.h"
#include "EMTableImport.h"

#include "libxl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportLibXL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableImportLibXL : public EMTableImport
{
public:
	virtual ~EMTableImportLibXL()
	{
		Clear();
	}

	libxl::Book* GetXLBook() { return xlBook;};

	virtual void Clear() override;
	virtual bool DoImport(const FString& Path) override;

protected:
	libxl::Book* xlBook;
};