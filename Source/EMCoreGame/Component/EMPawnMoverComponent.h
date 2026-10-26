#pragma once

#include "CoreMinimal.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "EMPawnMoverComponent.generated.h"

UCLASS(Abstract)
class EMCOREGAME_API UEMPawnMoverComponent : public UCharacterMoverComponent
{
	GENERATED_BODY()

public:
	UEMPawnMoverComponent();

	virtual FVector ConsumeInputVector() const;

	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
};