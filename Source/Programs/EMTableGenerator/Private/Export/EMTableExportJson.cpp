#include "EMTableExportJson.h"
#include "DataTable/EMDataTable.h"
#include "Config/EMTableConfig.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Utility/EMTableUtility.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableExportJson
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableExportJson::EMTableExportJson() : EMTableExport()
{

}

bool EMTableExportJson::DoExport()
{
	if (true == EMTableConfig::Get().GetJsonExportSection().IsMerge)
	{
		return MergeExport();
	}
	return SingleExport();
}

FString EMTableExportJson::ConvertJsonString(const UDataTable* ExportDataTable)
{
	FString JsonTableString = FString::Printf(TEXT("{\r\n\"%s\": [\r\n"), *ExportDataTable->GetFName().ToString());
	for (TMap<FName, uint8*>::TConstIterator RowMapIter = ExportDataTable->GetRowMap().CreateConstIterator(); RowMapIter; ++RowMapIter)
	{
		FString StructToJsonObjectString;

		FJsonObjectConverter::UStructToJsonObjectString((const UStruct*)ExportDataTable->RowStruct,
			(const void*)RowMapIter->Value, StructToJsonObjectString, 0, 0);

		JsonTableString += StructToJsonObjectString + TEXT(",");
	}

	JsonTableString += FString::Printf(TEXT("]\r\n},\r\n"));
	return JsonTableString;
}

bool EMTableExportJson::MergeExport()
{
	const TArray<class UDataTable*>& DataTables = EMSheetBuilderMgr::Get().GetSheetBuilderTransform().GetDataTables();

	EM_LOG(EMLog, Display, TEXT("============ Json[%d] 내보내기 ============"), DataTables.Num());

	FString OutJsonString;
	double UassetPrevTime = FPlatformTime::Seconds();

	for (int32 i = 0; i < DataTables.Num(); i++)
	{
		OutJsonString += ConvertJsonString(DataTables[i]);
	}

	if (false == EMTableUtility::WriteUTF8FileW(*EMTableConfig::Get().GetJsonExportSection().GetFinalMergeFilesname(), OutJsonString))
	{
		EM_LOG(EMLog, Fatal, TEXT("Json 병합파일 \"%s.%s\" 저장에 실패했습니다."), *EMTableConfig::Get().GetJsonExportSection().MergeFilename,
			*EMTableConfig::Get().GetJsonExportSection().Extname);
	}
	else
	{
		EM_LOG(EMLog, Display, TEXT("Json 병합파일 \"%s.%s\" 저장 완료[%.4f]"), *EMTableConfig::Get().GetJsonExportSection().MergeFilename,
			*EMTableConfig::Get().GetJsonExportSection().Extname, FPlatformTime::Seconds() - UassetPrevTime);
	}

	return true;
}

bool EMTableExportJson::SingleExport()
{
	const TArray<class UDataTable*>& DataTables = EMSheetBuilderMgr::Get().GetSheetBuilderTransform().GetDataTables();

	EM_LOG(EMLog, Display, TEXT("============ Json[%d] 내보내기 ============"), DataTables.Num());

	for (int32 i = 0; i < DataTables.Num(); i++)
	{
		double UassetPrevTime = FPlatformTime::Seconds();

		FString OutJsonString = ConvertJsonString(DataTables[i]);

		FString JsonExportFileName = FString::Printf(TEXT("%s/%s.%s"), *EMTableConfig::Get().GetJsonExportSection().Path,
			*DataTables[i]->GetFName().ToString(), *EMTableConfig::Get().GetJsonExportSection().Extname);

		if (false == EMTableUtility::WriteUTF8FileW(*JsonExportFileName, OutJsonString))
		{
			EM_LOG(EMLog, Fatal, TEXT("Json 파일 \"%s.%s\" 저장에 실패했습니다."), *DataTables[i]->GetFName().ToString(),
				*EMTableConfig::Get().GetJsonExportSection().Extname);
			return false;
		}
		else
		{
			EM_LOG(EMLog, Display, TEXT("Json 파일 \"%s.%s\" 저장 완료[%.4f]"), *DataTables[i]->GetFName().ToString(),
				*EMTableConfig::Get().GetJsonExportSection().Extname, FPlatformTime::Seconds() - UassetPrevTime);
		}
	}
	return true;
}