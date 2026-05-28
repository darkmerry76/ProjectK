#pragma once

#include "CoreMinimal.h"
#include "Core/EMCoreGameEngine.h"
#include "KMGameEngine.generated.h"

UCLASS()
class KMGAME_API UKMGameEngine : public UEMCoreGameEngine
{
	GENERATED_BODY()
public:
	virtual void Init(IEngineLoop* InEngineLoop) override;
};
