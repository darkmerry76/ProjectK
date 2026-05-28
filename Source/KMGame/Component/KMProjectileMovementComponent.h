#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "KMProjectileMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKMProjectileArrivalDelegate, const class UKMProjectileMovementComponent*, projectileMovementComponent);
UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_UCLASS_BODY()

public:
	FKMProjectileArrivalDelegate& GetProjectileArrivalDelegate();

	const FTransform& GetDeltaTransform() const;
	
protected:
	virtual FVector ComputeHomingAcceleration(const FVector& InVelocity, float DeltaTime) const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess="true"))
	FKMProjectileArrivalDelegate OnProjectileArrival;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="IsArrivalDestroyOwnerActor", meta=(AllowPrivateAccess="true"))
	bool IsArrivalDestroyActor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FTransform DeltaTransform;
};