#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KMPlacementPreviewActor.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class AKMPlacementPreviewActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USceneComponent* RootScene;

public:
	class USceneComponent* GetRootScene() const;
};