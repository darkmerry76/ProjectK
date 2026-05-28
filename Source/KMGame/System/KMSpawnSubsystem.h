#pragma once

#include "CoreMinimal.h"
#include "System/EMGameObjectSubsystem.h"
#include "KMSpawnSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMSpawnSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMSpawnSubsystem* GetSpawnSubsystem(const UObject* worldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Spawn System")
	class UKMHeroInstance* SpawnHero(const FName& heroId);

	class UKMActorInstance* SpawnProjectile(TSubclassOf<class AActor> actorClass, FTransform transform);

protected:
	void OnSkillProjectileTrigger(const TSharedPtr<class FKMSkillInstance>& skillInstance);
	
	UFUNCTION()
	void OnSkillProjectileEvent(FGameplayTag eventTag, const FKMProjectileEventData& EventData);
};