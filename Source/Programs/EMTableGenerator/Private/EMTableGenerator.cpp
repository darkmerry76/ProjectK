#include "EMTableGenerator.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "Config/EMTableConfig.h"
#include "Core/Public/Modules/ModuleManager.h"
#include "Launch/Public/LaunchEngineLoop.h"
#include "Utility/EMTableFeedbackContext.h"
#include "Windows/WindowsPlatformTime.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Utility/EMTableUtility.h"

DEFINE_LOG_CATEGORY(EMLog);
IMPLEMENT_APPLICATION(EMTableGenerator, "EMTableGenerator");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	FTaskTagScope Scope(ETaskTag::EGameThread);
	ON_SCOPE_EXIT
	{ 
		LLM(FLowLevelMemTracker::Get().UpdateStatsPerFrame());
		RequestEngineExit(TEXT("Exiting"));
		FEngineLoop::AppPreExit();
		FModuleManager::Get().UnloadModulesAtShutdown();
		FEngineLoop::AppExit();
	};
	
	GEngineLoop.PreInit(ArgC, ArgV);

	FPlatformTime::InitTiming();

	FString InitFilename = TEXT("default.ini");
	if (ArgC > 1)
	{
		GWorkPath = EMTableUtility::GetFullPathToPathname(ArgV[1]);
		InitFilename = EMTableUtility::GetFullPathToFilenameExt(ArgV[1]);
	}
	FString FinalIniFilename = FString::Printf(TEXT("%s%s"), *GWorkPath, *InitFilename);

	system("Color");

	GWarn = nullptr;
	if (false == EMTableConfig::Get().Init(FinalIniFilename))
	{
		EM_LOG(EMLog, Error, TEXT("Init File :[%s] 읽기에 실패했습니다.!!!"), *InitFilename);
		EMTableGrammar::FatalExceptionCount = 1;
		return EMTableUtility::ShutdownApplication();
	}

	FTableFeedbackContext TableFeedbackContext;

	GWarn = &TableFeedbackContext;

	EM_LOG(EMLog, Display, TEXT("Ini 파일 : %s"), *InitFilename);

	UObject* DefaultObject = UDataTable::StaticClass()->ClassDefaultObject;
	DefaultObject->GetOuter()->Rename(TEXT("/Script/Engine"));

	EMSheetBuilderMgr::Get().Process();
	
	return EMTableUtility::ShutdownApplication();
}
