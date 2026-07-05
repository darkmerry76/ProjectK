#pragma once

#include "CoreMinimal.h"
#include "EMGameInstanceSubsystem.h"
#include "EMWorldSubsystem.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EEMWorldLoadingState : uint8
{
	None,
	Ready,
	Loading,
	Complete,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_MULTICAST_DELEGATE(FEMWorldLoadingCompleteDelegate);
UCLASS(Blueprintable, BlueprintType, Abstract)
class EMCOREGAME_API UEMWorldSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FEMWorldLoadingCompleteDelegate LoadingCompleteDelegate;
	
	EEMWorldLoadingState GetLoadingState() const;
	
protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual void OnLevelAdded(ULevel* level, UWorld* world);
	virtual void OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS);
	virtual void OnPostWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS);
	virtual void OnPostLoadMapWithWorld(UWorld* loadedWorld);
	virtual void OnLoadingComplete();

protected:
	virtual bool IsAllStreamingLevelLoaded() const;
	void OnAllLevelsChanged();

protected:
	EEMWorldLoadingState LoadingState = EEMWorldLoadingState::None;
};