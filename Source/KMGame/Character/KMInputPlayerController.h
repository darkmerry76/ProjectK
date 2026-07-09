#pragma once

#include "CoreMinimal.h"
#include "KMInputPlayerController.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMInputPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void OnEscapePressed();
};
