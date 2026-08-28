#include "KMAbilityPaired.h"

#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMCharacterInstance.h"
#include "Util/KMUtil.h"

UKMAbilityPaired::UKMAbilityPaired(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityPaired::Activate()
{
	Super::Activate();
}

void UKMAbilityPaired::PostActivated()
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
	
	UKMCharacterInstance* casterCharacterInstance = GetCasterCharacterInstance();
	if (!IsValid(casterCharacterInstance))
	{
		return;
	}

	AKMCharacter* casterCharacter = casterCharacterInstance->GetCharacter();
	if (!IsValid(casterCharacter))
	{
		return;
	}
	
	if (bIsDirectionFallow)
	{
		float movementCircularDirection = ownerCharacterInstance->GetDirection();
		movementCircularDirection = UKMUtil::GetCircularAngle2D8Way(FVector2D(casterCharacter->GetActorForwardVector() * DirectionWeight));
		/*if (FMath::IsNearlyZero(casterCharacter->GetLatestMoveInputVelocity().Size()))
		{
			movementCircularDirection = UKMUtil::GetCircularAngle2D8Way(FVector2D(casterCharacter->GetActorForwardVector() * DirectionWeight));
		}
		else
		{
			movementCircularDirection = UKMUtil::GetCircularAngle2D8Way(FVector2D(casterCharacter->GetLatestMoveInputVelocity() * DirectionWeight));
		}*/
		ownerCharacterInstance->SetDirection(movementCircularDirection, bIsForceRotation);
	}
}

void UKMAbilityPaired::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
	
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}
}