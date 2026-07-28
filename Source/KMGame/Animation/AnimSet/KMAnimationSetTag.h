#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMAnimationSetTag.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class KMGAME_API UKMAnimationSetTag : public UDataAsset
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> AnimMontageMap;

	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> AnimOverrideMontageMap;

public:
	UAnimMontage* GetAnimation(FGameplayTag tag) const;

	UFUNCTION(BlueprintCallable)
	void SetMovementOverrideMontage(class UAnimMontage* jumpMontage, class UAnimMontage* landingMontage);

	UFUNCTION(BlueprintCallable)
	void RemoveMovementOverrideMontage();

};