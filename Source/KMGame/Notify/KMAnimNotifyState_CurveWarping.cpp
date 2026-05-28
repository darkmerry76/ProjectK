#include "KMAnimNotifyState_CurveWarping.h"
#include "EMCurveWarpingComponent.h"
#include "Character/KMCharacter.h"

UKMAnimNotifyState_CurveWarping::UKMAnimNotifyState_CurveWarping(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAnimNotifyState_CurveWarping::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(MeshComp->GetOwner());
	if(!IsValid(ownerCharacter))
	{
		return;
	}
	
	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	check(IsValid(curveWarping));

	FTransform curveWarpingTransform = curveWarping->GetCurveWarpingTargetTransform(TargetLocationName);
	curveWarping->PlayCurveWarpjng(Curve, curveWarpingTransform.GetLocation(), EventReference.GetNotify()->Duration, JumpScale, false, false);
}

void UKMAnimNotifyState_CurveWarping::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
}

void UKMAnimNotifyState_CurveWarping::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
}