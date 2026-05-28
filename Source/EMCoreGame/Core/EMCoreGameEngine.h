#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "EMCoreGameEngine.generated.h"

UCLASS()
class EMCOREGAME_API UEMCoreGameEngine : public UGameEngine
{
	GENERATED_BODY()
public:
	virtual void Init(IEngineLoop* InEngineLoop) override;
};
