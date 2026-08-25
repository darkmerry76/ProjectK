#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "KMCameraActor.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract, hidecategories=Object)
class AKMCameraActor : public ACameraActor
{
	GENERATED_UCLASS_BODY()
public:

	static const FIntPoint defaultViewportSize;

	virtual void BeginPlay() override;
	
	void SetViewportOnAspectRatio(class FViewport* viewport);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Camera)
	void OnPostViewportOnAspectRatio();
};