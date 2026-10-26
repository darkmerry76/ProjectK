#include "EMPawnMoverComponent.h"

UEMPawnMoverComponent::UEMPawnMoverComponent() : Super()
{
}

FVector UEMPawnMoverComponent::ConsumeInputVector() const
{
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(ownerPawn))
	{
		return FVector::ZeroVector;
	}
	return ownerPawn->Internal_ConsumeMovementInputVector();
}

void UEMPawnMoverComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	//FVector inputVector = ConsumeInputVector();
}