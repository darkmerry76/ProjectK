#include "KMAnimNotifyState_Animation.h"

UKMAnimNotifyState_Animation::UKMAnimNotifyState_Animation(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	GroupType = EEMNotifyGroupType::Animation;
}

void UKMAnimNotifyState_Animation::NotifyBegin(class USkeletalMeshComponent * meshComp, class UAnimSequenceBase * animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	
}

void UKMAnimNotifyState_Animation::NotifyTick(class USkeletalMeshComponent * meshComp, class UAnimSequenceBase * animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	
}

void UKMAnimNotifyState_Animation::NotifyEnd(class USkeletalMeshComponent * meshComp, class UAnimSequenceBase * animation, const FAnimNotifyEventReference& eventReference)
{
	
}
