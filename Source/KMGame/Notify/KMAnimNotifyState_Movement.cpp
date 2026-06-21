#include "KMAnimNotifyState_Movement.h"
#include "Character/KMCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UKMAnimNotifyState_Movement::UKMAnimNotifyState_Movement(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::Animation);
}

void UKMAnimNotifyState_Movement::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(MeshComp->GetOwner()))
	{
		ownerCharacter->GetCharacterMovement()->SetMovementMode(MovementMode);
	}
}

void UKMAnimNotifyState_Movement::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(MeshComp->GetOwner()))
	{
		ownerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}