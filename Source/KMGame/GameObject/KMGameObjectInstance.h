#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/KMGameplayTag.h"
#include "GameObject/EMGameObjectInstance.h"
#include "KMGameObjectInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMGameObjectInstance : public UEMGameObjectInstance
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void AddGameplayTag(FGameplayTag newTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnAddGameplayTag(const FGameplayTag& newTag);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveGameplayTag(FGameplayTag removedTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnRemoveGameplayTag(const FGameplayTag& removedTag);

	UFUNCTION(BlueprintPure)
	virtual bool HasGameplayTag(FGameplayTag tag) const;

	UFUNCTION(BlueprintPure)
	int32 GetGameplayTagCount(FGameplayTag tag) const;

protected:
	FKMGameplayTagContainer GameplayTagContainer;
};