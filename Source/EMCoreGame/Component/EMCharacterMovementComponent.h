#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EMCharacterMovementComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class EMCOREGAME_API UEMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()
};