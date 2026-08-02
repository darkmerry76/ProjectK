#include "KMAnimNotify_Stiff.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Character/KMCharacter.h"

UKMAnimNotify_Stiff::UKMAnimNotify_Stiff() : Super()
{
	GroupType = EEMNotifyGroupType::Plan; 
}

FString UKMAnimNotify_Stiff::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	notifyName += FString::Printf(TEXT("-'%.3f'"), Duration);
	
	return notifyName;
}

void UKMAnimNotify_Stiff::DoStiff(USkeletalMeshComponent* meshComp, float stiffDuration, bool bForce)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UKMCharacterInstance* ownerCharacterInstance = ownerCharacter->GetCharacterInstance();
	if (!IsValid(ownerCharacterInstance))
	{
		return;
	}
	ownerCharacterInstance->Stiff(stiffDuration, bForce);
}

void UKMAnimNotify_Stiff::Notify(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	Super::Notify(meshComp, animation, eventReference);

	DoStiff(meshComp, Duration, bIsForce);
}