#include "KMAnimNotifyState_MotionWarping.h"
#include "EMMartialArts.h"
#include "RootMotionModifier.h"

UKMAnimNotifyState_MotionWarping::UKMAnimNotifyState_MotionWarping(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	FLinearColor newNotifyColor = UEMMartialArts::GetBackGroundColor(GroupType);
	newNotifyColor *= 10.f;
	newNotifyColor.A = 1.f;
	NotifyColor = newNotifyColor.ToFColor(false);	
#endif
}

FString UKMAnimNotifyState_MotionWarping::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (URootMotionModifier_Warp* modifierWarp = Cast<URootMotionModifier_Warp>(RootMotionModifier))
	{
		if (modifierWarp->WarpTargetName.IsValid())
		{
			notifyName += FString::Printf(TEXT("-'%s'"), *modifierWarp->WarpTargetName.ToString());
		}
	}
	
	return notifyName;
}

void UKMAnimNotifyState_MotionWarping::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyBegin(meshComp, animation, totalDuration, eventReference);
}

void UKMAnimNotifyState_MotionWarping::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyTick(meshComp, animation, frameDeltaTime, eventReference);
}

void UKMAnimNotifyState_MotionWarping::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyEnd(meshComp, animation, eventReference);
}