#pragma once

#include "CoreMinimal.h"
#include "EMTransformUpdateInterface.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class EMCOREGAME_API UEMTransformUpdateInterface : public UInterface
{
	GENERATED_BODY()
};

class EMCOREGAME_API IEMTransformUpdateInterface
{
	GENERATED_BODY()

public:
	virtual void UpdateTransform(float deltaTime) = 0;
};