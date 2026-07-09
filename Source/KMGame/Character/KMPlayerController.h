#pragma once

#include "CoreMinimal.h"
#include "KMInputPlayerController.h"
#include "KMPlayerController.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMPlayerController : public AKMInputPlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<class UInputMappingContext*> DefaultMappingContexts;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnEscapePressed() override;
};
