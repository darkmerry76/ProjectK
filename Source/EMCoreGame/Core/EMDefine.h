#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EMDefine.generated.h"

DECLARE_DYNAMIC_DELEGATE_FourParams(FEMTweenDynamicDelegate, float, startValue, float, nextValue, float, elapsedTime, float, duration);
DECLARE_DELEGATE_FourParams(FEMTweenDelegate, float startValue, float nextValue, float elapsedTime, float duration);

UENUM(BlueprintType)
enum class EEMCharacterAnimationId : uint8
{
	Custom = 0,
	Idle,
	Walk,
	Run,
	Attack,
	Hit,
	Knockback,
	Knockdown,
	Dead,
};