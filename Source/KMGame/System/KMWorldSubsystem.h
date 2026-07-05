#pragma once

#include "CoreMinimal.h"
#include "System/EMWorldSubsystem.h"
#include "KMWorldSubsystem.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMWorldSubsystem : public UEMWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "worldContextObject"))
	static UKMWorldSubsystem* GetWorldSubsystem(const UObject* worldContextObject);

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual void OnLevelAdded(ULevel* level, UWorld* world) override;
	virtual void OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS) override;
	virtual void OnPostWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS) override;
	virtual void OnLoadingComplete() override;
};