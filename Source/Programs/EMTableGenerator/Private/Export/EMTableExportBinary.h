#pragma once

#include "EMTableGenerator.h"
#include "EMTableExport.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableExportBinary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableExportBinary : public EMTableExport
{
public:
	EMTableExportBinary();

protected:
	virtual bool DoExport() override;

protected:
	bool MergeExport();
	bool SingleExport();
	bool WriteExport(const FString& ExportPath, const EMTableCore::FEMTableMemoryFile& MemFile);
};