#pragma once

#include "CoreMinimal.h"
#include "Character/EMCharacterAnimInstance.h"
#include "KMCharacterAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMCharacterAnimInstance : public UEMCharacterAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeUpdateAnimation(float deltaSeconds) override;
};