#include "EMTableExportUAsset.h"
#include "DataTable/EMDataTable.h"
#include "Config/EMTableConfig.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "CoreUObject/Public/UObject/Package.h"
#include "CoreUObject/Public/UObject/SavePackage.h"
#include "Misc/FeedbackContext.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableExportUAsset
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableExportUAsset::EMTableExportUAsset() : EMTableExport()
{
}

bool EMTableExportUAsset::DoExport()
{
	if (true == EMTableConfig::Get().GetUAssetExportSection().IsMerge)
	{
		return MergeExport();
	}
	return SingleExport();
}

bool EMTableExportUAsset::MergeExport()
{
	const TArray<class UDataTable*>& DataTables = EMSheetBuilderMgr::Get().GetSheetBuilderTransform().GetDataTables();

	EM_LOG(EMLog, Display, TEXT("============ UDataTable[%d] 내보내기 ============"), DataTables.Num());

	UDataTable* ExportDataTable = DataTables[0];

	double UassetPrevTime = FPlatformTime::Seconds();

	UPackage* Package = Cast<UPackage>(ExportDataTable->GetOuter());
	if (nullptr == Package)
	{
		EM_LOG(EMLog, Fatal, TEXT("DataTable \"%s\" Outer(UPackage)가 null입니다."), *ExportDataTable->GetFName().ToString());

		return false;
	}

	FSavePackageResultStruct PackageResult = ESavePackageResult::Success;

	FSavePackageArgs SavePackageArgs;
	SavePackageArgs.TopLevelFlags = RF_Public | RF_Standalone | RF_Transactional;
	PackageResult =UPackage::Save(Package, ExportDataTable, *FString::Printf(TEXT("%s/%s.uasset"),
				*EMTableConfig::Get().GetUAssetExportSection().Path, *EMTableConfig::Get().GetUAssetExportSection().MergeFilename), SavePackageArgs);

	if (ESavePackageResult::Success != PackageResult.Result)
	{
		EM_LOG(EMLog, Error, TEXT("UDataTable \"%s\" 패키지 저장에 실패했습니다.[ESavePackageResult=%d]"),
			*ExportDataTable->GetFName().ToString(), (int32)PackageResult.Result);
	}
	else
	{
		EM_LOG(EMLog, Display, TEXT("UDataTable \"%s\" 패키지 저장 완료[%.4f]"),
			*ExportDataTable->GetFName().ToString(), FPlatformTime::Seconds() - UassetPrevTime);
	}
	return true;
}

bool EMTableExportUAsset::SingleExport()
{
	const TArray<class UDataTable*>& DataTables = EMSheetBuilderMgr::Get().GetSheetBuilderTransform().GetDataTables();

	EM_LOG(EMLog, Display, TEXT("============ UDataTable[%d] 내보내기 ============"), DataTables.Num());

	for (int32 i = 0; i < DataTables.Num(); i++)
	{
		UDataTable* ExportDataTable = DataTables[i];

		double UassetPrevTime = FPlatformTime::Seconds();

		UPackage* Package = Cast<UPackage>(ExportDataTable->GetOuter());
		if (nullptr == Package)
		{
			EM_LOG(EMLog, Fatal, TEXT("DataTable \"%s\" Outer(UPackage)가 null입니다."), *ExportDataTable->GetFName().ToString());
			return false;
		}

		FSavePackageResultStruct PackageResult = ESavePackageResult::Success;

		FSavePackageArgs SavePackageArgs;
		SavePackageArgs.TopLevelFlags = RF_Public | RF_Standalone | RF_Transactional;
		PackageResult = UPackage::Save(Package, ExportDataTable, *FString::Printf(TEXT("%s/%s.uasset"),
					*EMTableConfig::Get().GetUAssetExportSection().Path, *ExportDataTable->GetName()), SavePackageArgs);

		if (ESavePackageResult::Success != PackageResult.Result)
		{
			EM_LOG(EMLog, Error, TEXT("UDataTable \"%s\" 패키지 저장에 실패했습니다.[ESavePackageResult=%d]"),
				*ExportDataTable->GetFName().ToString(), (int32)PackageResult.Result);
		}
		else
		{
			EM_LOG(EMLog, Display, TEXT("UDataTable \"%s\" 패키지 저장 완료[%.4f]"),
				*ExportDataTable->GetFName().ToString(), FPlatformTime::Seconds() - UassetPrevTime);
		}
	}

	return true;
}

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif