#include "KMAnimNotifyState_JumpSection.h"
#include "Component/KMMartialArtsComponent.h"

UKMAnimNotifyState_JumpSection::UKMAnimNotifyState_JumpSection(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	SetGroupType(EEMNotifyGroupType::Animation);
#endif
}

FString UKMAnimNotifyState_JumpSection::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (SectionName != NAME_None)
	{
		notifyName += FString::Printf(TEXT("-%s'"), *SectionName.ToString());		
	}
	return notifyName;
}

void UKMAnimNotifyState_JumpSection::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetSkeletalMeshComponent(meshComp);

	const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>();
	if (!martialArtsData)
	{
		return;
	}
}

void UKMAnimNotifyState_JumpSection::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_JumpSection::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
}