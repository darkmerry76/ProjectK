#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_Misc.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAnimNotifyState_Misc : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere)
	bool bIsOverride_GlobalTimedilation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition=bIsOverride_GlobalTimedilation))
	float GlobalTimeDilationScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition=bIsOverride_GlobalTimedilation))
	TObjectPtr<UCurveFloat> GlobalTimeDilationCurve;

	float ElapsedTime = 0.f;
	float TotalDuration = 0.f;
	
protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	void SetTimeDilation(USkeletalMeshComponent* meshComp, float newTimeDilation);
};
