#include "KMGameEngine.h"
#include "Android/AndroidPlatformApplicationMisc.h"

void UKMGameEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);

#if PLATFORM_ANDROID
	FPlatformApplicationMisc::ControlScreensaver(FGenericPlatformApplicationMisc::Disable);
#endif
}

