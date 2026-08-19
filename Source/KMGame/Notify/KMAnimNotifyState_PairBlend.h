#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_PairBlend.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_PairBlend
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play PairBlend")
class KMGAME_API UKMAnimNotifyState_PairBlend : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="StartBlend", Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	bool bIsStartBlend = false;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(EditCondition="bIsStartBlend", AllowPrivateAccess=true, DisplayAfter="bIsStartBlend"))
	float StartBlendTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="StartBlendTime"))
	FVector PairOffset = FVector::ZeroVector; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="StopBlend", Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="PairOffset"))
	bool bIsStopBlend = false;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(EditCondition="bIsStopBlend", AllowPrivateAccess=true, DisplayAfter="bIsStopBlend"))
	float StopBlendTime = 0.1f;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};
