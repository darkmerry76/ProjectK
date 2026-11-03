#include "KMGameEngine.h"
#include "Android/AndroidPlatformApplicationMisc.h"

void UKMGameEngine::Init(IEngineLoop* engineLoop)
{
	Super::Init(engineLoop);

#if PLATFORM_ANDROID
	FPlatformApplicationMisc::ControlScreensaver(FGenericPlatformApplicationMisc::Disable);
#endif
}

void UKMGameEngine::Tick(float deltaSeconds, bool bIdleMode)
{
	Super::Tick(deltaSeconds, bIdleMode);
}

