#include "KMPawnMoverComponent.h"
#include "KMCurveWarpingComponent.h"
#include "Component/EMMoverCurveWarpingFallingMode.h"
#include "Curves/CurveVector.h"
#include "GameActor/Pawn/Character/KMPawnBase.h"

UKMPawnMoverComponent::UKMPawnMoverComponent() : Super()
{
}

void UKMPawnMoverComponent::OnRegister()
{
	Super::OnRegister();
	OnMovementModeChanged.AddDynamic(this, &UKMPawnMoverComponent::OnMoverModeChanged);
}

void UKMPawnMoverComponent::JumpCurveWarping()
{
	if (!IsValid(JumpCurve))
	{
		return;
	}

	AKMPawnBase* ownerPawn = Cast<AKMPawnBase>(GetOwner());
	if (!IsValid(ownerPawn))
	{
		return;
	}

	UKMCurveWarpingComponent* curveWarping = ownerPawn->GetCurveWarpingComponent();
	if (!IsValid(curveWarping))
	{
		return;
	}
	LatestJumpInputDir = ownerPawn->GetLastMovementInputVector();
	LatestJumpInputDir.Normalize();

	FVector targetLocation = ownerPawn->GetActorLocation() + (LatestJumpInputDir * JumpHorizontalDistance);

	float minValue = 0.f, maxValue = 1.f;
	JumpCurve->GetValueRange(minValue, maxValue);

	float zScale = JumpApexHeight / maxValue;

	ownerPawn->StartJump();
	curveWarping->PlayCurveWarping(EEMCustomMovementMode::CMODE_Falling, JumpCurve, targetLocation, JumpDuration,  zScale, false);
}

void UKMPawnMoverComponent::SetCustomWalkingAnimation(UAnimSequence* animSequence)
{
	CustomWalkingAnimSequence = animSequence;
}

void UKMPawnMoverComponent::ClearCustomWalkingAnimation()
{
	CustomWalkingAnimSequence = nullptr;
}

void UKMPawnMoverComponent::EnableCustomWalking()
{
	bIsEnableCustomWalking = true;
}

void UKMPawnMoverComponent::DisableCustomWalking()
{
	bIsEnableCustomWalking = false;
}

void UKMPawnMoverComponent::StartFollowActor(AActor* newLeaderActor, FName leaderMontageInstanceId, FName followMontageInstanceId, const FTransform& offsetTransform, float duration)
{
/*	if (!IsValid(newLeaderActor))
	{
		return;
	}
	
	AKMCharacter* followerCharacter = GetOwnerCharacter();
	AKMCharacter* leaderCharacter = Cast<AKMCharacter>(newLeaderActor);
	FollowerMovementData.State = EKMFollowerMovementStateType::Paried;
	FollowerMovementData.LeaderActor = newLeaderActor;
	FollowerMovementData.FollowActor = GetOwner();
	FollowerMovementData.LeaderMontageInstanceId = leaderMontageInstanceId;
	FollowerMovementData.FollowMontageInstanceId = followMontageInstanceId;
	FollowerMovementData.OffsetTransform = offsetTransform;
	FollowerMovementData.StartWorldTransform = FollowerMovementData.FollowActor->GetActorTransform();
	FollowerMovementData.ElipsedTime = 0.f;
	FollowerMovementData.HeightOffset = 0.f;
	FollowerMovementData.Duration = duration;

	float maxRadius = 0.f;
	float maxHeight = 0.f;

	if (IsValid(followerCharacter))
	{
		UKMAnimInstance* followerAnimInstance = Cast<UKMAnimInstance>(followerCharacter->GetMesh()->GetAnimInstance());
		check(IsValid(followerAnimInstance));

		FollowerMovementData.StartWorldTransform = followerCharacter->GetMesh()->GetComponentToWorld() * followerAnimInstance->GetPairBlendInfo().OffsetTransform;
		followerAnimInstance->SetPairOffsetTransform(FollowerMovementData);
		//followerCharacter->SetAnimRootMotionTranslationScale(0.f);

		//maxRadius = followerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		//maxHeight = followerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	}
	
	if (IsValid(leaderCharacter))
	{
		float prevHeight = leaderCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		
		if (maxRadius > leaderCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius())
		{
			leaderCharacter->GetCapsuleComponent()->SetCapsuleRadius(maxRadius);
		}
		else
		{
			maxRadius = leaderCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		}
		
		if (maxHeight > leaderCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
		{
			leaderCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(maxHeight);
		}
		else
		{
			maxHeight = leaderCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		}

		FVector newLeaderMeshRelativeLocation = leaderCharacter->GetMesh()->GetRelativeLocation();
		newLeaderMeshRelativeLocation.Z -= maxHeight - prevHeight;
		leaderCharacter->GetMesh()->SetRelativeLocation(newLeaderMeshRelativeLocation);
		leaderCharacter->SetActorLocation(leaderCharacter->GetActorLocation() + FVector(0.f, 0.f, maxHeight - prevHeight));
		
		FollowerMovementData.HeightOffset = followerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - leaderCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	if (IsValid(followerCharacter))
	{
		FVector newLeaderMeshRelativeLocation = leaderCharacter->GetMesh()->GetRelativeLocation();
		if (maxHeight > leaderCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
		{
			newLeaderMeshRelativeLocation.Z -= maxHeight - leaderCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		}
		
		leaderCharacter->GetMesh()->SetRelativeLocation(newLeaderMeshRelativeLocation);
	}*/
}

void UKMPawnMoverComponent::StopFollowActor(AActor* followActor, float duration)
{
/*	if (!FollowerMovementData.IsValid() || FollowerMovementData.LeaderActor != followActor)
	{
		return;
	}

	FollowerMovementData.State = EKMFollowerMovementStateType::Stoped;
	FollowerMovementData.Duration = duration;
	FollowerMovementData.ElipsedTime = 0.f;

	if (AKMCharacter* leaderCharacter = Cast<AKMCharacter>(FollowerMovementData.LeaderActor))
	{
		if (UKMCapsuleComponent* leaderCapsuleComponent = Cast<UKMCapsuleComponent>(leaderCharacter->GetCapsuleComponent()))
		{
			leaderCapsuleComponent->RevertOrigin();	
		}
		
		if (UKMSkeletalMeshComponent* leaderSkeletalMeshComponent = Cast<UKMSkeletalMeshComponent>(leaderCharacter->GetMesh()))
		{
			leaderSkeletalMeshComponent->RevertOriginTransform();
		}
	}

	if (AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor))
	{
		followerCharacter->SetActorLocation(FVector(FollowerMovementData.LatestRootWorldTransform.GetLocation().X, FollowerMovementData.LatestRootWorldTransform.GetLocation().Y, followerCharacter->GetActorLocation().Z));
		if (UKMAnimInstance* followerAnimInstance = Cast<UKMAnimInstance>(followerCharacter->GetMesh()->GetAnimInstance()))
		{
			followerAnimInstance->SetStopOffsetTransform();
		}

		if (UKMCapsuleComponent* followerCapsuleComponent = Cast<UKMCapsuleComponent>(followerCharacter->GetCapsuleComponent()))
		{
			followerCapsuleComponent->RevertOrigin();
		}

		if (UKMSkeletalMeshComponent* followerSkeletalMeshComponent = Cast<UKMSkeletalMeshComponent>(followerCharacter->GetMesh()))
		{
			followerSkeletalMeshComponent->RevertOriginTransform();
		}
		
		FollowerMovementData.Reset();
		followerCharacter->SetAnimRootMotionTranslationScale(1.f);
	}*/
}

/*void UKMPawnMoverComponent::MoveFollowProcessing(float deltaTime, int32 iterations)
{
	if (!FollowerMovementData.IsValid())
	{
		return;
	}
	
	if (FollowerMovementData.State == EKMFollowerMovementStateType::Paried)
	{
		FollowerMovementData.FollowActor->SetActorLocation(FVector(FollowerMovementData.LeaderActor->GetActorLocation().X, FollowerMovementData.LeaderActor->GetActorLocation().Y, FollowerMovementData.FollowActor->GetActorLocation().Z));
		FollowerMovementData.FollowActor->SetActorRotation((FollowerMovementData.LeaderActor->GetActorForwardVector() * -1.f).Rotation());

		if (AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor))
		{
			FollowerMovementData.LatestRootWorldTransform = followerCharacter->GetMesh()->GetSocketTransform(TEXT("Root"));
		}
	}
	
	else if (FollowerMovementData.State == EKMFollowerMovementStateType::Stoped)
	{
		if (AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor))
		{
			if (UKMAnimInstance* followerAnimInstance = Cast<UKMAnimInstance>(followerCharacter->GetMesh()->GetAnimInstance()))
			{
				//followerCharacter->SetActorLocation(FVector(FollowerMovementData.LatestRootWorldTransform.GetLocation().X, FollowerMovementData.LatestRootWorldTransform.GetLocation().Y, followerCharacter->GetActorLocation().Z));
				//FollowerMovementData.Reset();
				//followerCharacter->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}

	FollowerMovementData.ElipsedTime += deltaTime;
}*/


void UKMPawnMoverComponent::RegisterMoveBlockReflection(AKMCharacter* targetCharacter)
{
/*	TSharedPtr<FKMBlockReflectionData>* existBlockReflectData = BlockReflections.FindByPredicate([targetCharacter](const TSharedPtr<FKMBlockReflectionData>& blockReflectionData)
	{
		return blockReflectionData->Character == targetCharacter;
	});

	if (existBlockReflectData && (*existBlockReflectData).IsValid())
	{
		(*existBlockReflectData)->RefCount++;
	}
	else
	{
		TSharedPtr<FKMBlockReflectionData> newBlockReflectionData = MakeShared<FKMBlockReflectionData>();
		newBlockReflectionData->Character = targetCharacter;

		BlockReflections.Emplace(newBlockReflectionData);
	}*/
}

void UKMPawnMoverComponent::UnregisterMoveBlockReflection(AKMCharacter* targetCharacter)
{
/*	int32 existIndex = BlockReflections.IndexOfByPredicate([targetCharacter](const TSharedPtr<FKMBlockReflectionData>& blockReflectionData)
	{
		return blockReflectionData->Character == targetCharacter;
	});
	
	if (existIndex == INDEX_NONE || !BlockReflections[existIndex].IsValid())
	{
		return;
	}
	if (--BlockReflections[existIndex]->RefCount > 0)
	{
		return;
	}
	
	BlockReflections.RemoveAt(existIndex);*/
}

void UKMPawnMoverComponent::OnMoverModeChanged(const FName& previousMode, const FName& newMode)
{
	AKMPawnBase* ownerPawn = Cast<AKMPawnBase>(GetOwner());
	if (!IsValid(ownerPawn))
	{
		return;
	}

	if (previousMode != UEMMoverCurveWarpingFallingMode::ModeName && newMode == TEXT("Falling"))
	{
		ownerPawn->StartJump();		
	}
}