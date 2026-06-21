#include "KMAnimNotifyState_CurveWarping.h"
#include "EMCurveWarpingComponent.h"
#include "Character/KMCharacter.h"

UKMAnimNotifyState_CurveWarping::UKMAnimNotifyState_CurveWarping(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::Animation);
}

FString UKMAnimNotifyState_CurveWarping::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (!TargetLocationName.IsValid())
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *TargetLocationName.ToString());		
	}
	return notifyName;
}

void UKMAnimNotifyState_CurveWarping::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner());
	if(!IsValid(ownerCharacter))
	{
		return;
	}
	
	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	check(IsValid(curveWarping));

	FTransform curveWarpingTransform = curveWarping->GetCurveWarpingTargetTransform(TargetLocationName);
	curveWarping->PlayCurveWarpjng(Curve, curveWarpingTransform.GetLocation(), eventReference.GetNotify()->Duration, JumpScale, false, false);
}

void UKMAnimNotifyState_CurveWarping::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_CurveWarping::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
}