#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "KMAnimNotifyState_GameplayTag.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] GameplayTag")
class KMGAME_API UKMAnimNotifyState_GameplayTag : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	FGameplayTagContainer TagContainer;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};
