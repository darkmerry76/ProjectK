#include "KMAnimNotify_Sound.h"
#include "EMMartialArts.h"

UKMAnimNotify_Sound::UKMAnimNotify_Sound() : Super()
{
#if WITH_EDITOR
	FLinearColor newNotifyColor = UEMMartialArts::GetBackGroundColor(GroupType);
	newNotifyColor *= 10.f;
	newNotifyColor.A = 1.f;
	NotifyColor = newNotifyColor.ToFColor(false);	
#endif
}

FString UKMAnimNotify_Sound::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (IsValid(Sound))
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *Sound->GetName());
	}
	
	return notifyName;
}

void UKMAnimNotify_Sound::Notify(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	Super::Notify(meshComp, animation, eventReference);
}