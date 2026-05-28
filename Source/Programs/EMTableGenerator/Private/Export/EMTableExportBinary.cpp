#include "EMTableExportBinary.h"
#include "Config/EMTableConfig.h"
#include "Programs/EMTableGenerator/Private/Utility/EMTableUtility.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "Utility/EMTableGrammar.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableExportBinary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableExportBinary::EMTableExportBinary() : EMTableExport()
{

}

bool EMTableExportBinary::DoExport()
{
	if (true == EMTableConfig::Get().GetBinaryExportSection().IsMerge)
	{
		return MergeExport();
	}
	return SingleExport();
}

bool EMTableExportBinary::WriteExport(const FString& ExportPath, const EMTableCore::FEMTableMemoryFile& MemFile)
{
	if (false == EMTableUtility::WriteFile(*ExportPath, (const uint8*)MemFile.GetFileBuffer(), MemFile.GetFileLength()))
	{
		EM_LOG(EMLog, Error, TEXT("Binary[%s]저장중 쓰기에 실패했습니다.!!!"), *EMTableUtility::GetFullPathToFilenameExt(ExportPath));
		return false;
	}
	EM_LOG(EMLog, Display, TEXT("Binary[%s] 저장 완료!!"), *EMTableUtility::GetFullPathToFilenameExt(ExportPath));
	return true;
}

bool EMTableExportBinary::MergeExport()
{
	EM_LOG(EMLog, Display, TEXT("============ Binary병합 내보내기 ============"));
	EMTableCore::FEMTableMemoryFile MemFile(100000000);

	EMTableCore::FEMTableBinaryWriter BinaryWriter(&MemFile);

	for (int32 i = 0; i < EMSheetBuilderMgr::Get().GetSheetBuilders().Num(); i++)
	{
		BinaryWriter.AddSheet(EMSheetBuilderMgr::Get().GetSheetBuilders()[i]);
	}
	BinaryWriter.Write(EMTableConfig::Get().GetBinaryExportSection().IsCompress);

	WriteExport(EMTableConfig::Get().GetBinaryExportSection().GetFinalMergeFilesname(), MemFile);

	return true;
}

bool EMTableExportBinary::SingleExport()
{
	EM_LOG(EMLog, Display, TEXT("============ Binary[%d] 개별로 내보내기 ============"), EMSheetBuilderMgr::Get().GetSheetBuilders().Num());
	for (int32 i = 0; i < EMSheetBuilderMgr::Get().GetSheetBuilders().Num(); i++)
	{
		TSharedPtr<EMSheetBuilder> CurrSheet =  EMSheetBuilderMgr::Get().GetSheetBuilders()[i];

		EMTableCore::FEMTableMemoryFile MemFile(100000000);

		EMTableCore::FEMTableBinaryWriter BinaryWriter(&MemFile);

		BinaryWriter.AddSheet(EMSheetBuilderMgr::Get().GetSheetBuilders()[i]);

		BinaryWriter.Write(EMTableConfig::Get().GetBinaryExportSection().IsCompress);

		WriteExport(EMTableConfig::Get().GetBinaryExportSection().GetFinalSingleFilename(*CurrSheet->GetSheetName()), MemFile);
	}

	return true;
}