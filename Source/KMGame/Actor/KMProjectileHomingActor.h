#pragma once

#include "CoreMinimal.h"
#include "KMProjectileActorBase.h"
#include "KMProjectileHomingActor.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract, hidecategories=Object)
class AKMProjectileHomingActor : public AKMProjectileActorBase
{
	GENERATED_UCLASS_BODY()
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure, Category="Projectile")
	class UKMProjectileMovementComponent* GetProjectileMovementComponent() const;

	UFUNCTION(BlueprintPure, Category="Projectile")
	AActor* GetTarget() const;

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category="Projectile")
	void SetTarget(AActor* newTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Projectile")
	void OnTargetArrival();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category="Projectile")
	void SetCreatedIndex(int32 index);

	void AddArrivalData(TSharedPtr<struct FKMProjectileArrivalData> newArrivalData);
	int32 NumArrivalData() const;
	TSharedPtr<struct FKMProjectileArrivalData> GetArrivalDataByIndex(int32 index) const;

protected:
	UPROPERTY(Category="Projectile", EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	class UKMProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(Category="Projectile", EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	AActor* Target;
	
	UPROPERTY(Category="Projectile", EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	int32 CreatedIndex;

	TArray<TSharedPtr<struct FKMProjectileArrivalData>> ArrivalData;
};