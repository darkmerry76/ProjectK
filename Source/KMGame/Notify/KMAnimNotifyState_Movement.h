#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_Movement.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Movement")
class KMGAME_API UKMAnimNotifyState_Movement : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	TEnumAsByte<EMovementMode> MovementMode = EMovementMode::MOVE_Walking;
	
protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};
