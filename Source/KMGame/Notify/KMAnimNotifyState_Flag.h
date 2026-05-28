#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_Flag.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAnimNotifyState_Flag : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMAbilityFlag AbilityFlag = EKMAbilityFlag::None;

public:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
