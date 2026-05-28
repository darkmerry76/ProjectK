#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_CurveWarping.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAnimNotifyState_CurveWarping : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveBase* Curve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TargetLocationName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpScale = 1.f;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
