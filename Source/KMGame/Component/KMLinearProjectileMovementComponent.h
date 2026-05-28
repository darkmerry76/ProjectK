#pragma once

#include "CoreMinimal.h"
#include "KMLinearProjectileMovementComponent.generated.h"

DECLARE_DELEGATE(FKMLinearProjectileArrival)

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMLinearProjectileMovementComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:
	FKMLinearProjectileArrival ArrivalDelegate;
	
protected:
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Distance = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* Curve = nullptr;

protected:
	float ElapsedTime = 0.f;

	bool bFinished = false;
};