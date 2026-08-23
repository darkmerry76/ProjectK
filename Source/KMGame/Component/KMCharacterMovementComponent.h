#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingInterface.h"
#include "Component/EMCharacterMovementComponent.h"
#include "EMCurveWarpingComponent.h"
#include "KMCharacterMovementComponent.generated.h"

struct KMGAME_API FKMBlockReflectionData
{
	TWeakObjectPtr<class AKMCharacter> Character;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKMCustomMovementDelegate, float, deltaTime, int32, iterations);
DECLARE_MULTICAST_DELEGATE_OneParam(FKMSweepPawnHitDelegate, const TArray<FHitResult>& hitResults);

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMCharacterMovementComponent : public UEMCharacterMovementComponent, public IEMCurveWarpingInterface
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FKMCustomMovementDelegate CustomMovementDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCurveVector> JumpCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpDuration = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpApexHeight = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpHorizontalDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UClass* ActorClassFilter;

	float FallTime = 0.f;

	FVector LatestJumpInputDir = FVector::ZeroVector;

	FKMSweepPawnHitDelegate SweepPawnHitDelegate;

	UPROPERTY()
	TWeakObjectPtr<class UAnimSequence> CustomWalkingAnimSequence;

	bool bIsEnableCustomWalking = false;

	TArray<TSharedPtr<FKMBlockReflectionData>> BlockReflections;

	FHitResult BlockHitResult;

	bool bHasResolvedBlockMove = false;
	FVector BlockMoveDelta = FVector::ZeroVector;

protected:
	UPROPERTY()
	class AActor* FollowActor = nullptr;

public:
	void CustomJump();

	virtual bool CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations) override;

	bool IsOnGround() const;
	bool IsAir() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetCustomWalkingAnimation(class UAnimSequence* animSequence);

	UFUNCTION(BlueprintCallable)
	virtual void ClearCustomWalkingAnimation();

	UFUNCTION(BlueprintCallable)
	virtual void EnableCustomWalking();

	UFUNCTION(BlueprintCallable)
	virtual void DisableCustomWalking();

	UFUNCTION(BlueprintCallable)
	virtual void StartFollowActor(class AActor* newFollowerActor, const FVector& targetOffset, float duration = 0.2f);

	UFUNCTION(BlueprintCallable)
	virtual void StopFollowActor(float duration = 0.2f);

	UFUNCTION(BlueprintPure)
	bool IsCustomWalking() const;

	UFUNCTION(BlueprintCallable)
	void RegisterMoveBlockReflection(class AKMCharacter* targetCharacter);

	UFUNCTION(BlueprintCallable)
	void UnregisterMoveBlockReflection(class AKMCharacter* targetCharacter);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	
	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *thisTickFunction) override;
	virtual void PhysCustom(float deltaTime, int32 iterations) override;
	virtual void PhysWalking(float deltaTime, int32 iterations) override;
	virtual void HandleImpact(const FHitResult& impact, float timeSlice, const FVector& moveDelta) override;
	virtual void MoveBlockProcessing(float deltaTime, int32 iterations);
	virtual void OnMovementUpdated(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity) override;
	virtual void ProcessLanded(const FHitResult& hit, float remainingTime, int32 iterations) override;
	virtual void StartNewPhysics(float deltaTime, int32 iterations) override;

	bool CustomMovementFalling(const FVector& adjusted, float deltaTime, int32 iterations);
	bool CustomMovementFlying(const FVector& adjusted, float deltaTime, int32 iterations);
	bool CustomMovementWalking(const FVector& adjusted, float deltaTime, int32 iterations);
	
	virtual void StartFalling(int32 iterations, float remainingTime, float timeTick, const FVector& delta, const FVector& subLoc) override;
	virtual void PhysFalling(float deltaTime, int32 iterations ) override;

	UFUNCTION()
	void OnJumpInterrupt(const FVector& moveDelta, float deltaTime, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType interuptType, EEMCustomMovementMode newMovementMode);

	class AKMCharacter* GetOwnerCharacter() const;
	class UKMCharacterInstance* GetOwnerCharacterInstance() const;

	void ProcessOverlapDamage(float deltaSeconds, const FVector& oldLocation, const FVector& newLocation);
	
};