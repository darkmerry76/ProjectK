#pragma once

#include "CoreMinimal.h"
#include "KMSpringArmComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMSpringArmComponent : public USceneComponent 
{
	GENERATED_UCLASS_BODY()
	
protected:
	FVector RelativeSocketLocation;
	FQuat RelativeSocketRotation;

	FTransform CameraTransform;
	
public:
	static const FName SocketName;

protected:
	virtual void OnRegister() override;
	virtual bool HasAnySockets() const override;
	virtual void QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const override;

public:
	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const override;;
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
};