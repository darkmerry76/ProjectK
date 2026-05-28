#include "EMTableImportLibXL.h"
#include "EMTableImportXML.h"
#include "Programs/EMTableGenerator/Private/EMTableGenerator.h"
#include "Programs/EMTableGenerator/Private/SheetBuilder/EMSheet.h"
#include "SheetBuilder/EMSheet.h"
#include "Utility/EMTableUtility.h"
#include "Windows/WindowsPlatformTime.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportLibXL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMTableImportLibXL::Clear()
{
	if(nullptr != xlBook)
	{
		xlBook->release();
		xlBook = nullptr;
	}
}

bool EMTableImportLibXL::DoImport(const FString& Path)
{
	double XMLSecondsScope = FPlatformTime::Seconds();

	xlBook = xlCreateXMLBook();
	xlBook->setKey(LIBXL_USERNAME, LIBXL_KEY);
	if (false == xlBook->load(*Path))
	{
		return false;
	}

	EM_LOG(EMLog, Display, TEXT("LibXL[%s] 불러오기 완료[%.4f]"), *Path, FPlatformTime::Seconds() - XMLSecondsScope);

	for (int32 i = 0; i < (int32)xlBook->sheetCount(); i++)
	{
		AddSheetAnalizer(EMTableUtility::GetFullPathToFilenameExt(Path), xlBook->getSheetName(i), MakeShareable(new EMSheetLibXL(xlBook->getSheet(i))));
	}

	return true;
}
