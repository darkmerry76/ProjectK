#include "EMTableImportXML.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "SheetBuilder/EMSheet.h"
#include "Templates/SharedPointer.h"
#include "Templates/UniquePtr.h"
#include "Utility/EMTableUtility.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportXML
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EMTableImportXML::Clear()
{
	if (nullptr != XMLDocument)
	{
		delete XMLDocument;
		XMLDocument = nullptr;
	}
}

bool EMTableImportXML::DoImport(const FString& Path)
{
	double XMLSecondsScope = FPlatformTime::Seconds();

	XMLDocument = new pugi::xml_document();

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Excel이 열려 있어도 읽기 가능한 모드
	TSharedPtr<IFileHandle> FileHandle(PlatformFile.OpenRead(*Path, true));
	if (FileHandle.IsValid() == false)
	{
		return false;
	}

	TArray<uint8> FileBuffer;

	FileBuffer.AddUninitialized(FileHandle->Size());
	FileHandle->Read(FileBuffer.GetData(), FileHandle->Size());
	FileHandle = nullptr;
	
	pugi::xml_parse_result Result = XMLDocument->load_buffer(FileBuffer.GetData(), FileBuffer.Num(), pugi::parse_default, pugi::encoding_utf8);
	if (Result.status != pugi::status_ok)
	{
		return false;
	}
	
	EM_LOG(EMLog, Display, TEXT("XML[%s] 불러오기 완료[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), FPlatformTime::Seconds() - XMLSecondsScope);

	pugi::xml_node pWorkbook = XMLDocument->child(TEXT("Workbook"));

	if (false == FCString::Stricmp(pWorkbook.name(), TEXT("Workbook")) && false == FCString::Stricmp(pWorkbook.name(), TEXT("ss:Workbook")))
	{
		return false;
	}
	for (auto& pWorksheet : pWorkbook)
	{
		if (false == FCString::Stricmp(pWorkbook.name(), TEXT("Workbook")) && false == FCString::Stricmp(pWorkbook.name(), TEXT("ss:Workbook")))
		{
			continue;
		}

		pugi::xml_attribute sheet = pWorksheet.attribute(TEXT("ss:Name"));
		const FString xlSheetName = sheet.value();

		pugi::xml_node CurrentSheet = pWorksheet.child(TEXT("Table"));
		if (!CurrentSheet)
			CurrentSheet = pWorksheet.child(TEXT("ss:Table"));
		if (!CurrentSheet)
			continue;

		AddSheetAnalizer(EMTableUtility::GetFullPathToFilenameExt(Path), xlSheetName, MakeShareable(new EMSheetXML(CurrentSheet)));
	}
	return true;
}
