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
/*	if (MovementMode == MOVE_Custom && newMovementMode != MOVE_Custom)
	{
		if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner()))
		{
			if (UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping())
			{
				curveWarping->ClearCurveWarping();
			}
		}
	}*/
	
	Super::SetMovementMode(newMovementMode, newCustomMode);
}

void UKMCharacterMovementComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *thisTickFunction)
{
	if (FMath::IsNearlyZero(deltaTime))
	{
		return;
	}
	FVector startLocation = UpdatedComponent->GetComponentLocation();
	
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
	
	const FVector adjusted = Velocity * deltaTime;
	FVector actualDelta = UpdatedComponent->GetComponentLocation() - startLocation;
	BlockMoveDelta = FVector(adjusted.X - actualDelta.X,adjusted.Y - actualDelta.Y,0.f);
	bHasResolvedBlockMove = true;

	MoveBlockProcessing(deltaTime, 1);
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
		TArray<AActor*> overlapActors;
		
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

void UKMCharacterMovementComponent::StartFalling(int32 iterations, float remainingTime, float timeTick, const FVector& delta, const FVector& subLoc)
{
	Super::StartFalling(iterations, remainingTime, timeTick, delta, subLoc);
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	ownerCharacter->MontqagePlayTag(FKMGameplayTagName::Anim_Jump_0);
}

void UKMCharacterMovementComponent::PhysFalling(float deltaTime, int32 iterations)
{
	Super::PhysFalling(deltaTime, iterations);
}

void UKMCharacterMovementComponent::PhysCustom(float deltaTime, int32 iterations)
{
	Super::PhysCustom(deltaTime, iterations);
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

void UKMCharacterMovementComponent::HandleImpact(const FHitResult& impact, float timeSlice, const FVector& moveDelta)
{
	Super::HandleImpact(impact, timeSlice, moveDelta);

	if (!impact.bBlockingHit || IsWalkable(impact))
	{
		return;
	}
	BlockHitResult = impact;
}

void UKMCharacterMovementComponent::RegisterMoveBlockReflection(AKMCharacter* targetCharacter)
{
	TSharedPtr<FKMBlockReflectionData> newBlockReflectionData = MakeShared<FKMBlockReflectionData>();
	newBlockReflectionData->Character = targetCharacter;
	
	bool bExist = BlockReflections.ContainsByPredicate([targetCharacter](const TSharedPtr<FKMBlockReflectionData>& blockReflectionData)
	{
		return blockReflectionData->Character == targetCharacter;
	});

	if (bExist)
	{
		return;
	}
	BlockReflections.Emplace(newBlockReflectionData);
}

void UKMCharacterMovementComponent::MoveBlockProcessing(float deltaTime, int32 iterations)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	USkeletalMeshComponent* ownerSkeletalMeshComponent = ownerCharacter->GetMesh();
	if (!IsValid(ownerSkeletalMeshComponent))
	{
		return;
	}

	UKMAnimInstance* ownerAnimInstance = Cast<UKMAnimInstance>(ownerSkeletalMeshComponent->GetAnimInstance());
	if (!IsValid(ownerAnimInstance))
	{
		return;
	}

	UEMCurveWarpingComponent* ownerCurveWarping = Cast<UEMCurveWarpingComponent>(ownerCharacter->GetCurveWarping());
	if (!IsValid(ownerCurveWarping))
	{
		return;
	}

	FVector totalMoveDalta = FVector::ZeroVector;
	int32 numBlocked = 0;
	for (auto blockReflection : BlockReflections)
	{
		if (!blockReflection->Character.IsValid())
		{
			continue;
		}
		
		UKMCharacterMovementComponent* targetCharacterMovement = Cast<UKMCharacterMovementComponent>(blockReflection->Character->GetCharacterMovement());
		if (!IsValid(targetCharacterMovement))
		{
			continue;
		}

		if (!targetCharacterMovement->BlockHitResult.bBlockingHit)
		{
			continue;
		}
		
		FVector blockedDelta = targetCharacterMovement->BlockMoveDelta;
		targetCharacterMovement->BlockHitResult.Reset();
		targetCharacterMovement->bHasResolvedBlockMove = false;

		totalMoveDalta += blockedDelta * -1.f;

		++numBlocked;
	}
	
	if (!totalMoveDalta.IsNearlyZero())
	{
		totalMoveDalta /= static_cast<float>(numBlocked);
		CustomMovementWalking(FVector(totalMoveDalta.X, totalMoveDalta.Y, 0.f), deltaTime, iterations);
		ownerCurveWarping->RemoveAllWarpTargets();
	}
}

void UKMCharacterMovementComponent::UnregisterMoveBlockReflection(AKMCharacter* targetCharacter)
{
	int32 existIndex = BlockReflections.IndexOfByPredicate([targetCharacter](const TSharedPtr<FKMBlockReflectionData>& blockReflectionData)
	{
		return blockReflectionData->Character == targetCharacter;
	});
	if (existIndex == INDEX_NONE)
	{
		return;
	}
	
	BlockReflections.RemoveAt(existIndex);

	if (BlockReflections.IsEmpty())
	{
		AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
		if (!IsValid(ownerCharacter))
		{
			return;
		}
		
		USkeletalMeshComponent* ownerSkeletalMeshComponent = ownerCharacter->GetMesh();
		if (!IsValid(ownerSkeletalMeshComponent))
		{
			return;
		}

		UKMAnimInstance* ownerAnimInstance = Cast<UKMAnimInstance>(ownerSkeletalMeshComponent->GetAnimInstance());
		if (!IsValid(ownerAnimInstance))
		{
			return;
		}
	}
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
		CustomMovementWalking(FVector(moveDirection.X, moveDirection.Y, GetGravityZ() * deltaTime * 0.5f), deltaTime, iterations);
		return;
	}
	
	Super::PhysWalking(deltaTime, iterations);
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
	LatestJumpInputDir = ownerCharacter->GetLastMovementInputVector();
	LatestJumpInputDir.Normalize();

	FVector targetLocation = GetActorLocation() + (LatestJumpInputDir * JumpHorizontalDistance);

	float minValue = 0.f, maxValue = 1.f;
	JumpCurve->GetValueRange(minValue, maxValue);

	float zScale = JumpApexHeight / maxValue;

	ownerCharacter->MontqagePlayTag(FKMGameplayTagName::Anim_Jump_0);
	curveWarping->PlayCurveWarping(EEMCustomMovementMode::CMODE_Falling, JumpCurve, targetLocation, JumpDuration,  zScale, false);
}

void UKMCharacterMovementComponent::OnJumpInterrupt(const FVector& moveDelta, float deltaTime, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType interuptType, EEMCustomMovementMode newMovementMode)
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("OnJumpInterrupt moveDelta=%s interuptType=%d newMovementMode=%d"), *moveDelta.ToString(), interuptType, newMovementMode));
	UE_LOG(LogTemp, Display, TEXT("OnJumpInterrupt moveDelta=%s interuptType=%d newMovementMode=%d"), *moveDelta.ToString(), interuptType, newMovementMode);

	if (newMovementMode == EEMCustomMovementMode::CMODE_Falling)
	{
		if (interuptType == EEMCurveWarpingInteruptType::Landing)
		{
			SetMovementMode(MOVE_Walking);
		}
		else
		{
			Velocity = moveDelta * (FMath::IsNearlyZero(deltaTime) ? 0.f : (1.f / deltaTime));
			SetMovementMode(MOVE_Falling);
		}
	}
}

void UKMCharacterMovementComponent::StartNewPhysics(float deltaTime, int32 iterations)
{
	Super::StartNewPhysics(deltaTime, iterations);
	CustomMovementDelegate.Broadcast(deltaTime, iterations);
}

bool UKMCharacterMovementComponent::CustomMovement(EEMCustomMovementMode movementMode, const FVector& adjusted, float deltaTime, int32 iterations)
{
	bool bResult = true;

	float deltaPerSeconds = (FMath::IsNearlyZero(deltaTime) ? 0.f : (1.f / deltaTime));
	if (!FMath::IsNearlyZero(adjusted.Size2D()))
	{
		Velocity.X = adjusted.X * deltaPerSeconds;
		Velocity.Y = adjusted.Y * deltaPerSeconds;
	}
	
	if (movementMode == EEMCustomMovementMode::CMODE_Walking)
	{
		bResult = CustomMovementWalking(adjusted, deltaTime, iterations);
	}
	else if (movementMode == EEMCustomMovementMode::CMODE_Falling)
	{
		Velocity.Z = adjusted.Z * deltaPerSeconds;
		bResult = CustomMovementFalling(adjusted, deltaTime, iterations);
	}
	else if (movementMode == EEMCustomMovementMode::CMODE_Flying)
	{
		bResult = CustomMovementFlying(adjusted, deltaTime, iterations);
	}

	//if (Velocity.Z < 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("%d Adjusted=%s Velocity=%s deltaTime=%.3f"), movementMode, *adjusted.ToString(), *Velocity.ToCompactString(), deltaTime));
		UE_LOG(LogTemp, Display, TEXT("%d Adjusted=%s Velocity=%s deltaTime=%.3f"), movementMode, *adjusted.ToString(), *Velocity.ToCompactString(), deltaTime);
	}

	return bResult;
}

bool UKMCharacterMovementComponent::CustomMovementWalking(const FVector& adjusted, float deltaTime, int32 iterations)
{
	FHitResult hitResult;
	FVector finalAdjusted = FVector(adjusted.X, adjusted.Y, adjusted.Z);
	SafeMoveUpdatedComponent(finalAdjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);
	if (hitResult.bBlockingHit)
	{
		FVector gravDir = FVector(0.f, 0.f, -1.f); 
        
		if (!StepUp(gravDir, finalAdjusted, hitResult, nullptr))
		{
			SlideAlongSurface(finalAdjusted, 1.f - hitResult.Time, hitResult.Normal, hitResult, true);
		}
	}
	return true;
}

void UKMCharacterMovementComponent::ProcessLanded(const FHitResult& hit, float remainingTime, int32 iterations)
{
	Super::ProcessLanded(hit, remainingTime, iterations);
}

bool UKMCharacterMovementComponent::CustomMovementFalling(const FVector& adjusted, float deltaTime, int32 iterations)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return false;
	}
	float remainingTime = deltaTime;
	int32 Iterations = 0.f;
	while( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) )
	{
		Iterations++;
		float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		FVector finalAdjusted = adjusted * (timeTick / deltaTime);
		FHitResult hitResult;
		SafeMoveUpdatedComponent(finalAdjusted, UpdatedComponent->GetComponentRotation(), true, hitResult);

		float subTimeTickRemaining = deltaTime * (1.f - hitResult.Time);
		float lastMoveTimeSlice = deltaTime;
		if (hitResult.bBlockingHit)
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult))
			{
				ProcessLanded(hitResult, remainingTime, Iterations);
				return false;
			}
			else
			{
				if (!hitResult.bStartPenetrating && ShouldCheckForValidLandingSpot(deltaTime, finalAdjusted, hitResult))
				{
					const FVector pawnLocation = UpdatedComponent->GetComponentLocation();
					FFindFloorResult floorResult;
					FindFloor(pawnLocation, floorResult, false);

					if (!floorResult.bLineTrace && finalAdjusted.Z < 0.f && floorResult.IsWalkableFloor() && IsValidLandingSpot(pawnLocation, floorResult.HitResult))
					{
						ProcessLanded(floorResult.HitResult, remainingTime, Iterations);
						return false;
					}
				}

				const FVector oldHitNormal = hitResult.Normal;
				const FVector oldHitImpactNormal = hitResult.ImpactNormal;				
				FVector delta = ComputeSlideVector(finalAdjusted, 1.f - hitResult.Time, oldHitNormal, hitResult);
				if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && (delta | finalAdjusted) > 0.f)
				{
					SafeMoveUpdatedComponent( delta, UpdatedComponent->GetComponentRotation(), true, hitResult);
					if (hitResult.bBlockingHit)
					{
						subTimeTickRemaining = subTimeTickRemaining * (1.f - hitResult.Time);

						if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hitResult))
						{
							ProcessLanded(hitResult, remainingTime, Iterations);
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
							ProcessLanded(hitResult, remainingTime, Iterations);
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}

bool UKMCharacterMovementComponent::CustomMovementFlying(const FVector& adjusted, float deltaTime, int32 iterations)
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
			//return true;
		}

		FFindFloorResult floorResult;
		//FindFloor(UpdatedComponent->GetComponentLocation(), FloorResult, false);

		ComputeFloorDist(
			UpdatedComponent->GetComponentLocation(),
	MaxStepHeight,
	MaxStepHeight,
		floorResult,
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(),
	nullptr);

		if (floorResult.IsWalkableFloor())
		{
			if (floorResult.FloorDist <= MAX_FLOOR_DIST)
			{
				return true;
			}
		}
	}

	return false;
}

bool UKMCharacterMovementComponent::IsAir() const
{
	if (MovementMode == MOVE_Custom)
	{
		return !IsOnGround();
	}
	return IsFalling();
}

void UKMCharacterMovementComponent::StartFollowActor(AActor* newFollowActor, const FVector& targetOffset, float duration)
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(ownerCharacter->GetMesh()->GetAnimInstance());
	if (!IsValid(animInstance))
	{
		return;
	}

	FollowActor = newFollowActor;
	
	FTransform startTransform = ownerCharacter->GetActorTransform();
	ownerCharacter->SetActorLocation(newFollowActor->GetActorLocation());
	animInstance->BlendPairPosition(startTransform, targetOffset, duration);
}

void UKMCharacterMovementComponent::StopFollowActor(float duration)
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(ownerCharacter->GetMesh()->GetAnimInstance());
	if (!IsValid(animInstance))
	{
		return;
	}
	
	FollowActor = nullptr;

	ownerCharacter->AddActorLocalOffset(animInstance->GetPairBlendInfo().FinalWorldPosition);
	animInstance->BlendPairPosition(ownerCharacter->GetActorTransform(), FVector::ZeroVector, duration);
}