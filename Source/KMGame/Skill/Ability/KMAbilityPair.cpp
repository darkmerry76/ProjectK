#include "KMAbilityPair.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMCharacterInstance.h"
#include "Util/KMUtil.h"

UKMAbilityPair::UKMAbilityPair(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityPair::Activate()
{
	Super::Activate();
}

void UKMAbilityPair::PostActivated()
{
	UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
	if (!IsValid(ownerCharacterInstance))
	{
		return;
	}

	AKMCharacter* ownerCharacter = ownerCharacterInstance->GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	if (bIsDirectionFallow)
	{
		float movementCircularDirection = ownerCharacterInstance->GetCharacterDirection(); 
		if (FMath::IsNearlyZero(ownerCharacter->GetLatestMoveInputVelocity().Size()))
		{
			movementCircularDirection = UKMUtil::GetCircularAngle2D8Way(FVector2D(ownerCharacter->GetActorForwardVector() * DirectionWeight));
		}
		else
		{
			movementCircularDirection = UKMUtil::GetCircularAngle2D8Way(FVector2D(ownerCharacter->GetLatestMoveInputVelocity() * DirectionWeight));
		}
		ownerCharacterInstance->SetCharacterDirection(movementCircularDirection, bIsForceRotation);
	}
}

void UKMAbilityPair::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
}