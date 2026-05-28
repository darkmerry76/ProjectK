#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "KMProjectileActorBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKMProjectileTriggerDelegate, FGameplayTag, eventTag, const FKMProjectileEventData&, eventData);
UCLASS(Blueprintable, BlueprintType, Abstract, hidecategories=Object)
class AKMProjectileActorBase : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FKMProjectileTriggerDelegate TriggerDeletgate;
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void TriggerEvent(FGameplayTag eventTag, const FKMProjectileEventData& EventData);

	TSharedPtr<class FKMSkillInstance> SkillInstance;

protected:
	FPrimaryAssetId GetPrimaryAssetId() const;
};