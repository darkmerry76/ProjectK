#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_Misc.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Misc")
class KMGAME_API UKMAnimNotifyState_Misc : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="IsGlobalTimedilation", Category=Dilation, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	bool bIsOverride_GlobalTimedilation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Dilation, meta=(EditCondition=bIsOverride_GlobalTimedilation, AllowPrivateAccess=true, DisplayAfter="bIsOverride_GlobalTimedilation"))
	float GlobalTimeDilationScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Dilation, meta=(EditCondition=bIsOverride_GlobalTimedilation, AllowPrivateAccess=true, DisplayAfter="GlobalTimeDilationScale"))
	TObjectPtr<UCurveFloat> GlobalTimeDilationCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Dilation, meta=(AllowPrivateAccess=true, DisplayAfter="GlobalTimeDilationCurve"))
	bool bIsOverride_ShowOwnerCharacter = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Dilation, meta=(AllowPrivateAccess=true, DisplayAfter="bIsOverride_ShowOwnerCharacter"))
	bool bIsOverride_ShowOtherCharacter = false;

	float ElapsedTime = 0.f;
	float TotalDuration = 0.f;
	
protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	void SetTimeDilation(USkeletalMeshComponent* meshComp, float newTimeDilation);
	void CollectionShowActor(AActor* newActor, TArray<AActor*>& showActors);

protected:
	virtual FString GetNotifyName_Implementation() const override;
};
