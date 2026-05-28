#include "KMAnimInstance.h"
#include "Character/KMCharacter.h"
#include "Util/KMUtil.h"

void UKMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	NextDirection = CurrentDirection;
}

void UKMAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);
	
	CurrentDirection = UKMUtil::FInterpToCircular(CurrentDirection, NextDirection, deltaSeconds, LerpDirectionSpeed);
	if(AKMCharacter* ownerCharacter = Cast<AKMCharacter>(TryGetPawnOwner()))
	{
		if (UKMCharacterInstance* characterInstance = ownerCharacter->GetCharacterInstance())
		{
			if (PrevActorDirection != characterInstance->GetCharacterDirection())
			{
				PrevActorDirection = characterInstance->GetCharacterDirection();
			}
		}
	}
}

#if WITH_EDITOR

void UKMAnimInstance::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	Super::PostEditChangeProperty(propertyChangedEvent);

	if (propertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimInstance, CurrentDirection))
	{
		NextDirection = CurrentDirection;
	}
}

void UKMAnimInstance::SetCurrentDirection(float newCurrentDirection)
{
	CurrentDirection = newCurrentDirection;	
}

float UKMAnimInstance::GetCurrentDirection() const
{
	return CurrentDirection;
}

void UKMAnimInstance::SetNextDirection(float newNextDirection)
{
	NextDirection = newNextDirection;
}

float UKMAnimInstance::GetNextDirection() const
{
	return NextDirection;
}

const TArray<FName>& UKMAnimInstance::GetHiddenBones() const
{
	return HiddenBones;
}

#endif