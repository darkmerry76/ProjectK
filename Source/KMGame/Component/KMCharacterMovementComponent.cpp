#include "KMCharacterMovementComponent.h"
#include "EMCurveWarpingComponent.h"
#include "Animation/AnimSequenceHelpers.h"
#include "Animation/KMAnimInstance.h"
#include "Character/KMCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetSystemLibrary.h"

const float MIN_TICK_DIST = 1.f;

UKMCharacterMovementComponent::UKMCharacterMovementComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	
	ObjectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ActorClassFilter = ACharacter::StaticClass();
}

AKMCharacter* UKMCharacterMovementComponent::GetOwnerCharacter() const
{
	return Cast<AKMCharacter>(GetOwner());
}

UKMCharacterInstance* UKMCharacterMovementComponent::GetOwnerCharacterInstance() const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return nullptr;
	}
	return ownerCharacter->GetCharacterInstance();
}

void UKMCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	if (!IsValid(curveWarping))
	{
		return;
	}

	if (!curveWarping->GetInteruptDelegate().IsAlreadyBound(this, &ThisClass::OnJumpInterrupt))
	{
		curveWarping->GetInteruptDelegate().AddDynamic(this, &ThisClass::OnJumpInterrupt);
	}
}

void UKMCharacterMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	if (!IsValid(curveWarping))
	{
		return;
	}

	if (curveWarping->GetInteruptDelegate().IsAlreadyBound(this, &ThisClass::OnJumpInterrupt))
	{
		curveWarping->GetInteruptDelegate().RemoveAll(this);
	}
}

void UKMCharacterMovementComponent::SetMovementMode(EMovementMode newMovementMode, uint8 newCustomMode)
{
	Super::SetMovementMode(newMovementMode, newCustomMode);
}

void UKMCharacterMovementComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
}

void UKMCharacterMovementComponent::ProcessOverlapDamage(float deltaSeconds, const FVector& oldLocation, const FVector& newLocation)
{
	if (!SweepPawnHitDelegate.IsBound())
	{
		return;
	}
	
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	FVector velocity = newLocation - oldLocation;
	
	if (velocity.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return;
	}

	float radius = ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float halfHeight = ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	TArray<AActor*> overlapActors;
	TArray<FHitResult> hitResults;
	if (FVector::Dist(newLocation, oldLocation) > radius * 0.5f)
	{
		
		FCollisionShape capsule = FCollisionShape::MakeCapsule(radius, halfHeight);

		FCollisionQueryParams params;
		params.AddIgnoredActor(ownerCharacter);
		params.bFindInitialOverlaps = true;

		GetWorld()->SweepMultiByChannel(hitResults, oldLocation, newLocation, ownerCharacter->GetActorQuat(),ECC_Pawn, capsule, params);
	}
	else
	{
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Emplace(ownerCharacter);
		UKismetSystemLibrary::CapsuleOverlapActors(this, newLocation, radius, halfHeight,  ObjectTypeQuery, ActorClassFilter, actorsToIgnore, overlapActors);

		for (auto actorItr : overlapActors)
		{
			if (!IsValid(actorItr) || ownerCharacter == actorItr)
			{
				continue;
			}

			AKMCharacter* hitharacter = Cast<AKMCharacter>(actorItr);
			if (!IsValid(hitharacter))
			{
				continue;
			}

			UCapsuleComponent* capsuleComp = Cast<UCapsuleComponent>(hitharacter->GetCapsuleComponent());
			if (!IsValid(capsuleComp))
			{
				continue;
			}
			
			FVector closestPoint;
			if (!capsuleComp->GetClosestPointOnCollision(newLocation, closestPoint))
			{
				closestPoint = hitharacter->GetActorLocation();
			}

			FHitResult newHitResult(hitharacter, capsuleComp, newLocation, velocity.GetSafeNormal());
			newHitResult.ImpactPoint = closestPoint;
			newHitResult.Location = closestPoint;
			newHitResult.TraceStart = oldLocation;
			newHitResult.TraceEnd = newLocation;
			
			hitResults.Emplace(newHitResult);
		}
	}

	if (SweepPawnHitDelegate.IsBound() && !hitResults.IsEmpty())
	{
		SweepPawnHitDelegate.Broadcast(hitResults);
	}
}

void UKMCharacterMovementComponent::OnMovementUpdated(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity)
{
	Super::OnMovementUpdated(deltaSeconds, oldLocation, oldVelocity);
	
	ProcessOverlapDamage(deltaSeconds, oldLocation, UpdatedComponent->GetComponentLocation());
}

void UKMCharacterMovementComponent::SetCustomMovementMode(EKMCustomMovementMode newCustomMovementMode)
{
	MovementModeEx = newCustomMovementMode;
}

bool UKMCharacterMovementComponent::IsCustomMovementMode(EKMCustomMovementMode customMovementMode) const
{
	return MovementModeEx == customMovementMode;
}

void UKMCharacterMovementComponent::StartFalling(int32 iterations, float remainingTime, float timeTick, const FVector& delta, const FVector& subLoc)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		Super::StartFalling(iterations, remainingTime, timeTick, delta,  subLoc);
		return;
	}
	
	SetMovementMode(MOVE_Custom);

	StartCustomFalling(Velocity);

	ownerCharacter->MontqagePlayTag(FKMGameplayTagName::Anim_Jump_0);
}

void UKMCharacterMovementComponent::StartCustomFalling(const FVector& latestMoveDelta)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter)) return;

	Velocity.X = latestMoveDelta.X;
	Velocity.Y = latestMoveDelta.Y;
	Velocity.Z = latestMoveDelta.Z;

	FallTime = 0.f;

	SetCustomMovementMode(EKMCustomMovementMode::CMODE_Falling);
}

void UKMCharacterMovementComponent::PhysCustom(float deltaTime, int32 iterations)
{
	Super::PhysCustom(deltaTime, iterations);

	CustomMovementDelegate.Broadcast(deltaTime, iterations);

	if (IsCustomMovementMode(EKMCustomMovementMode::CMODE_Falling))
	{
		UpdateCustomFalling(deltaTime);
	}
}

void UKMCharacterMovementComponent::SetCustomWalkingAnimation(UAnimSequence* animSequence)
{
	CustomWalkingAnimSequence = animSequence;
}

void UKMCharacterMovementComponent::ClearCustomWalkingAnimation()
{
	CustomWalkingAnimSequence = nullptr;
}

void UKMCharacterMovementComponent::EnableCustomWalking()
{
	bIsEnableCustomWalking = true;
}

void UKMCharacterMovementComponent::DisableCustomWalking()
{
	bIsEnableCustomWalking = false;
}

bool UKMCharacterMovementComponent::IsCustomWalking() const
{
	return CustomWalkingAnimSequence.IsValid() && !HasAnimRootMotion() && bIsEnableCustomWalking;
}

void UKMCharacterMovementComponent::PhysWalking(float deltaTime, int32 iterations)
{
	ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	if (CustomWalkingAnimSequence.IsValid() && !HasAnimRootMotion() && bIsEnableCustomWalking)
	{
		USkeletalMeshComponent* skeletalMeshComponent = ownerCharacter->GetMesh();
		if (!IsValid(skeletalMeshComponent))
		{
			return;
		}
		UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(skeletalMeshComponent->GetAnimInstance());
		if (!IsValid(animInstance))
		{
			return;
		}
		
		float syncMoveRunElipsedTime = animInstance->GetMovementElapsedTime();
		const float sequenceLength = CustomWalkingAnimSequence->GetPlayLength();
		
		float startTime = FMath::Fmod(syncMoveRunElipsedTime, sequenceLength);
		float endTime = startTime + deltaTime;

		FTransform rootMotion = FTransform::Identity;
		if (endTime <= sequenceLength)
		{
			rootMotion = UE::Anim::ExtractRootMotionFromAnimationAsset(CustomWalkingAnimSequence.Get(),nullptr, startTime, endTime);
		}
		else
		{
			const FTransform rootMotionA = UE::Anim::ExtractRootMotionFromAnimationAsset(CustomWalkingAnimSequence.Get(), nullptr, startTime, sequenceLength);
			const FTransform rootMotionB = UE::Anim::ExtractRootMotionFromAnimationAsset(CustomWalkingAnimSequence.Get(), nullptr, 0.f,endTime - sequenceLength);
			
			rootMotion = rootMotionA + rootMotionB;
		}

		bool bRun = syncMoveRunElipsedTime >= 0.19f && syncMoveRunElipsedTime <= (0.19f + 0.45f);
		float movementInput = ownerCharacter->GetLastMovementInputVector().IsNearlyZero() ? 0.f : 1.f;
		FVector moveDirection = ownerCharacter->GetActorForwardVector() * rootMotion.GetLocation().Size2D() * (bRun ? 2.5f : 1.f) * movementInput;
		
		Velocity = ownerCharacter->GetActorForwardVector() * 700.f * movementInput;
		CustomMovementWalking(FVector(moveDirection.X, moveDirection.Y, GetGravityZ() * deltaTime * 0.5f), deltaTime);
		return;
	}
	
	Super::PhysWalking(deltaTime, iterations);
}

void UKMCharacterMovementComponent::PlayCurveWarping(UCurveBase* newCurveAsset, FVector newTargetLocation, float newPlayLength, float newZScale, bool bIgnoreZ, bool bAutoEndingWalk)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	if (!IsValid(curveWarping))
	{
		return;
	}

	curveWarping->PlayCurveWarpjng(newCurveAsset, newTargetLocation, newPlayLength, newZScale, bIgnoreZ, bAutoEndingWalk);
	SetCustomMovementMode(EKMCustomMovementMode::CMODE_Flying);
}

void UKMCharacterMovementComponent::CustomJump()
{
	if (!IsValid(JumpCurve))
	{
		return;
	}

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	if (!IsValid(curveWarping))
	{
		return;
	}
	
	SetCustomMovementMode(EKMCustomMovementMode::CMODE_Jump);
	
	LatestJumpInputDir = ownerCharacter->GetLastMovementInputVector();
	LatestJumpInputDir.Normalize();

	FVector targetLocation = GetActorLocation() + (LatestJumpInputDir * JumpHorizontalDistance);

	float minValue = 0.f, maxValue = 1.f;
	JumpCurve->GetValueRange(minValue, maxValue);

	float zScale = JumpApexHeight / maxValue;

	ownerCharacter->MontqagePlayTag(FKMGameplayTagName::Anim_Jump_0);
	curveWarping->PlayCurveWarpjng(JumpCurve, targetLocation, JumpDuration,  zScale, false, false);
}

void UKMCharacterMovementComponent::StartCurveEndingFalling(const UCurveVector* curveVector)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	if (!IsValid(curveWarping))
	{
		return;
	}
	
	if (IsCustomMovementMode(EKMCustomMovementMode::CMODE_Jump) ||
		IsCustomMovementMode(EKMCustomMovementMode::CMODE_Falling))
	{
		SetCustomMovementMode(EKMCustomMovementMode::CMODE_Falling);
		if (IsValid(curveVector))
		{
			const float sampleTime = 1.f / 60.f;
			FVector v1 = curveVector->GetVectorValue(1.f - sampleTime);
			FVector v2 = curveVector->GetVectorValue(1.f);

			FVector CalculatedVelocity = (v2 - v1) / sampleTime;
			const FQuat meshRotation = curveWarping->GetStartMeshQuat();
			CalculatedVelocity = meshRotation.RotateVector(CalculatedVelocity);

			CalculatedVelocity = CalculatedVelocity * FVector(5.f * FMath::Abs(LatestJumpInputDir.X), 5.f * FMath::Abs(LatestJumpInputDir.Y), 1.f);
					
			StartCustomFalling(CalculatedVelocity);
		}
	}
	else
	{
		SetMovementMode(MOVE_Walking);
	}
}

void UKMCharacterMovementComponent::OnJumpInterrupt(const FVector& moveDelta, EEMCurveWarpingInteruptType type)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UKMCharacterInstance* ownerCharacterInstance = ownerCharacter->GetCharacterInstance();
	if (!IsValid(ownerCharacterInstance))
	{
		return;
	}

	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	if (!IsValid(curveWarping))
	{
		return;
	}
	
	switch (type)
	{
	case EEMCurveWarpingInteruptType::Ending: StartCurveEndingFalling(JumpCurve); break;
	case EEMCurveWarpingInteruptType::Landing:
		{
			ownerCharacter->MontqagePlayTag(FKMGameplayTagName::Anim_Landing_0);
			Velocity = FVector::ZeroVector;
			SetMovementMode(MOVE_Walking);
			SetCustomMovementMode(EKMCustomMovementMode::CMODE_Walking);
		}
		break;
	default: break;
	}
}

void UKMCharacterMovementComponent::UpdateCustomFalling(float deltaTime)
{
	FallTime += deltaTime;
	Velocity.Z += GetGravityZ() * deltaTime;

	UEMCurveWarpingComponent* curveWarping = nullptr;
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (IsValid(ownerCharacter))
	{
		curveWarping = ownerCharacter->GetCurveWarping();
	}

	const FVector adjusted = Velocity * deltaTime;
	if (!CustomMovement(adjusted, deltaTime))
	{
		if(IsValid(curveWarping))
		{
			curveWarping->GetInteruptDelegate().Broadcast(adjusted, EEMCurveWarpingInteruptType::Landing);	
		}
	}
}

bool UKMCharacterMovementComponent::CustomMovement(const FVector& adjusted, float deltaTime)
{
	if (IsCustomMovementMode(EKMCustomMovementMode::CMODE_Walking))
	{
		return CustomMovementWalking(adjusted, deltaTime);
	}
	else if (IsCustomMovementMode(EKMCustomMovementMode::CMODE_Falling) || IsCustomMovementMode(EKMCustomMovementMode::CMODE_Jump))
	{
		return CustomMovementFalling(adjusted, deltaTime);
	}
	else
	{
		return CustomMovementFlying(adjusted, deltaTime);
	}
}

bool UKMCharacterMovementComponent::CustomMovementWalking(const FVector& adjusted, float deltaTime)
{
	FHitResult hitResult;
	FVector finalAdjusted = FVector(adjusted.X, adjusted.Y, GetGravityZ() * deltaTime * 0.5f);
	SafeMoveUpdatedComponent(finalAdjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);
	if (hitResult.bBlockingHit)
	{
		FVector gravDir = FVector(0.f, 0.f, -1.f); 
        
		if (!StepUp(gravDir, adjusted, hitResult, nullptr))
		{
			SlideAlongSurface(adjusted, 1.f - hitResult.Time, hitResult.Normal, hitResult, true);
		}
	}
		
	FFindFloorResult floorResult;
	FindFloor(UpdatedComponent->GetComponentLocation(), floorResult, false);

	if (!floorResult.IsWalkableFloor())
	{
		SetCustomMovementMode(EKMCustomMovementMode::CMODE_Falling);
	}
	return true;
}

bool UKMCharacterMovementComponent::CustomMovementFalling(const FVector& adjusted, float deltaTime)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return false;
	}
	FHitResult hitResult;
	SafeMoveUpdatedComponent(adjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);

	float subTimeTickRemaining = deltaTime * (1.f - hitResult.Time);
	float lastMoveTimeSlice = deltaTime;
	if (hitResult.bBlockingHit)
	{
		if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult))
		{
			ownerCharacter->Landed(hitResult);
			return false;	
		}
		else
		{
			if (!hitResult.bStartPenetrating && ShouldCheckForValidLandingSpot(deltaTime, adjusted, hitResult))
			{
				const FVector pawnLocation = UpdatedComponent->GetComponentLocation();
				FFindFloorResult floorResult;
				FindFloor(pawnLocation, floorResult, false);

				if (!floorResult.bLineTrace && adjusted.Z < 0.f && floorResult.IsWalkableFloor() && IsValidLandingSpot(pawnLocation, floorResult.HitResult))
				{
					ownerCharacter->Landed(floorResult.HitResult);
					return false;
				}
			}

			const FVector oldHitNormal = hitResult.Normal;
			const FVector oldHitImpactNormal = hitResult.ImpactNormal;				
			FVector delta = ComputeSlideVector(adjusted, 1.f - hitResult.Time, oldHitNormal, hitResult);
			if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && (delta | adjusted) > 0.f)
			{
				SafeMoveUpdatedComponent( delta, UpdatedComponent->GetComponentRotation(), true, hitResult);
				if (hitResult.bBlockingHit)
				{
					subTimeTickRemaining = subTimeTickRemaining * (1.f - hitResult.Time);

					if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult))
					{
						ownerCharacter->Landed(hitResult);
						return false;
					}
					lastMoveTimeSlice = subTimeTickRemaining;
					HandleImpact(hitResult, lastMoveTimeSlice, delta);
					TwoWallAdjust(delta, hitResult, oldHitNormal);

					bool bDitch = ( (GetGravitySpaceZ(oldHitImpactNormal) > 0.f) &&
						(GetGravitySpaceZ(hitResult.ImpactNormal) > 0.f) && (FMath::Abs(GetGravitySpaceZ(delta)) <= UE_KINDA_SMALL_NUMBER) && ((hitResult.ImpactNormal | oldHitImpactNormal) < 0.f) );

					SafeMoveUpdatedComponent( delta, UpdatedComponent->GetComponentRotation(), true, hitResult);
					if (hitResult.Time == 0.f )
					{
						FVector sideDelta = ProjectToGravityFloor(oldHitNormal + hitResult.ImpactNormal).GetSafeNormal();
						if(sideDelta.IsNearlyZero())
						{
							if (HasCustomGravity())
							{
								const FVector gravityRelativeHitNormal = RotateWorldToGravity(oldHitNormal);
								sideDelta = RotateGravityToWorld(FVector(gravityRelativeHitNormal.Y, -gravityRelativeHitNormal.X, 0.f)).GetSafeNormal();
							}
							else
							{
								sideDelta = FVector(oldHitNormal.Y, -oldHitNormal.X, 0).GetSafeNormal();	
							}
						}
						SafeMoveUpdatedComponent(sideDelta, UpdatedComponent->GetComponentRotation(), true, hitResult);
					}
					if(bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult) || hitResult.Time == 0.f)
					{
						ownerCharacter->Landed(hitResult);
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool UKMCharacterMovementComponent::CustomMovementFlying(const FVector& adjusted, float deltaTime)
{
    ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
    if (!IsValid(ownerCharacter)) return true;
    
    FHitResult hitResult;
    SafeMoveUpdatedComponent(adjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);
	
	if (hitResult.IsValidBlockingHit())
	{
		FVector gravDir = FVector(0.f, 0.f, -1.f); 
        
		if (!StepUp(gravDir, adjusted, hitResult, nullptr))
		{
			SlideAlongSurface(adjusted, 1.f - hitResult.Time, hitResult.Normal, hitResult, true);
		}
	}

    return true;
}

bool UKMCharacterMovementComponent::IsOnGround() const
{
	if (MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking)
	{
		return true;
	}

	if (MovementMode == MOVE_Custom)
	{
		if (CurrentFloor.IsWalkableFloor() && CurrentFloor.FloorDist <= MIN_TICK_DIST)
		{
			return true;
		}

		FFindFloorResult FloorResult;
		FindFloor(UpdatedComponent->GetComponentLocation(), FloorResult, false);

		if (FloorResult.IsWalkableFloor())
		{
			if (FloorResult.FloorDist <= MaxStepHeight) 
			{
				return true;
			}
		}
	}

	return false;
}

bool UKMCharacterMovementComponent::IsAir() const
{
	return !IsOnGround();
}