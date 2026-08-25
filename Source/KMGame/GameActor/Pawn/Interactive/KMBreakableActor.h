#pragma once

#include "CoreMinimal.h"
#include "KMInteractiveActorBase.h"
#include "KMBreakableActor.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMBreakableActor : public AKMInteractiveActorBase
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> StaticMesh;
};