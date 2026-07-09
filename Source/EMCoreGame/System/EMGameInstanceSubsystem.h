#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EMGameInstanceSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UEMGameInstanceSubsystem* GetSubsystem(
		const UObject* worldContextObject, TSubclassOf<UGameInstanceSubsystem> subsystemClass);

	virtual void Initialize();
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category="GameInstanceSubsystem", meta=(AllowPrivateAccess=true))
	void OnInitalize();

protected:
	virtual void OnLevelAdded(ULevel* level, UWorld* world);
	virtual void OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS);
	virtual void OnPostWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS);
	virtual void OnWorldCleanup(UWorld* cleaupWorld, bool bSessionEnded, bool bCleanupResources);
	virtual void OnPostLoadMapWithWorld(UWorld* loadedWorld);

private:
	virtual void Initialize(FSubsystemCollectionBase& collection) override;
};