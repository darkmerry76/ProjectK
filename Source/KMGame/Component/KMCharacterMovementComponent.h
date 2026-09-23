#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingInterface.h"
#include "Component/EMCharacterMovementComponent.h"
#include "EMCurveWarpingComponent.h"
#include "Component/EMTransformUpdateInterface.h"
#include "Core/KMDefine.h"
#include "KMCharacterMovementComponent.generated.h"

struct KMGAME_API FKMBlockReflectionData
{
	int32 RefCount = 1;
	bool bHasResolvedBlockMove = false;
	TWeakObjectPtr<class AKMCharacter> Character;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FKMSweepPawnHitDelegate, const TArray<FHitResult>& hitResults);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMCharacterMovementComponent : public UEMCharacterMovementComponent, public IEMCurveWarpingInterface, public IEMTransformUpdateInterface
{
	GENERATED_UCLASS_BODY()
	
public:
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
	FEMCustomMovementDelegate CustomMovementDelegate;

	UPROPERTY()
	TWeakObjectPtr<class UAnimSequence> CustomWalkingAnimSequence;

	bool bIsEnableCustomWalking = false;

	TArray<TSharedPtr<FKMBlockReflectionData>> BlockReflections;

	FKMFollowerMovementData FollowerMovementData;

	FHitResult BlockHitResult;
	FVector BlockMoveDelta = FVector::ZeroVector;

public:
	void CustomJump();

	virtual void BindCurveWarpingComponent(class UEMCurveWarpingComponent* newCurveWarpingComponent) override;
	virtual void UnbindCurveWarpingComponent(class UEMCurveWarpingComponent* newCurveWarpingComponent) override;
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
	virtual void StartFollowActor(class AActor* newFollowerActor, FName leaderMontageInstanceId, FName followMontageInstanceId, const FTransform& offsetTransform, float duration = 0.2f);

	UFUNCTION(BlueprintCallable)
	virtual void StopFollowActor(class AActor* followActor, float duration = 0.2f);

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
	virtual void PerformMovement(float deltaTime) override;
	virtual void UpdateBasedMovement(float deltaTime) override;
	virtual void TickCharacterPose(float deltaTime) override;
	virtual void PhysCustom(float deltaTime, int32 iterations) override;
	virtual void PhysWalking(float deltaTime, int32 iterations) override;
	virtual void HandleImpact(const FHitResult& impact, float timeSlice, const FVector& moveDelta) override;
	virtual void MoveBlockProcessing(float deltaTime, int32 iterations);
	virtual void MoveFollowProcessing(float deltaTime, int32 iterations);
	virtual void OnMovementUpdated(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity) override;
	virtual void ProcessLanded(const FHitResult& hitResult, float remainingTime, int32 iterations) override;
	virtual void ProcessWallHit(const FHitResult hitResult);
	virtual void ProcessCeilingHit(const FHitResult hitResult);
	virtual void StartNewPhysics(float deltaTime, int32 iterations) override;
	virtual void UpdateTransform(float deltaTime) override;
	
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