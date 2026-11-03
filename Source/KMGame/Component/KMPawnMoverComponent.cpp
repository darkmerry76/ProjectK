#include "KMPawnMoverComponent.h"
#include "KMCurveWarpingComponent.h"
#include "KMMoveShapeComponent.h"
#include "KMSkeletalMeshComponent.h"
#include "Animation/KMAnimInstance.h"
#include "Component/EMMoverCurveWarpingFallingMode.h"
#include "Curves/CurveVector.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameActor/Pawn/Character/KMPawnBase.h"

UKMPawnMoverComponent::UKMPawnMoverComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKMPawnMoverComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnMovementModeChanged.AddDynamic(this, &UKMPawnMoverComponent::OnMoverModeChanged);
	DefaultBaseVisualTransform = BaseVisualComponentTransform;
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

AKMCharacter* UKMPawnMoverComponent::GetOwnerCharacter() const
{
	return Cast<AKMCharacter>(GetOwner());
}

void UKMPawnMoverComponent::StartFollowActor(AActor* newLeaderActor, FName leaderMontageInstanceId, FName followMontageInstanceId, const FTransform& offsetTransform, float duration)
{
	if (!IsValid(newLeaderActor))
	{
		return;
	}
	
	AKMCharacter* followerCharacter = GetOwnerCharacter();
	if (!IsValid(followerCharacter))
	{
		return;
	}
	
	AKMCharacter* leaderCharacter = Cast<AKMCharacter>(newLeaderActor);
	if (!IsValid(leaderCharacter))
	{
		return;
	}

	UKMAnimInstance* leaderAnimInstance = Cast<UKMAnimInstance>(leaderCharacter->GetMesh()->GetAnimInstance());
	if(!IsValid(leaderAnimInstance))
	{
		return;
	}

	UKMAnimInstance* followerAnimInstance = Cast<UKMAnimInstance>(followerCharacter->GetMesh()->GetAnimInstance());
	if(!IsValid(followerAnimInstance))
	{
		return;
	}
	
	FollowerMovementData.State = EKMFollowerMovementStateType::Started;
	FollowerMovementData.LeaderActor = newLeaderActor;
	FollowerMovementData.FollowActor = GetOwner();
	FollowerMovementData.LeaderMontageInstanceId = leaderMontageInstanceId;
	FollowerMovementData.FollowMontageInstanceId = followMontageInstanceId;
	FollowerMovementData.OffsetTransform = offsetTransform;
	FollowerMovementData.StartWorldTransform = FollowerMovementData.FollowActor->GetActorTransform();
	FollowerMovementData.ElipsedTime = 0.f;
	FollowerMovementData.HeightOffset = 0.f;
	FollowerMovementData.Duration = duration;
}

bool UKMPawnMoverComponent::StartedFollowActor()
{
	AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor);
	if (!IsValid(followerCharacter))
	{
		return false;
	}

	AKMCharacter* leaderCharacter = Cast<AKMCharacter>(FollowerMovementData.LeaderActor);
	if (!IsValid(leaderCharacter))
	{
		return false;
	}

	UKMAnimInstance* leaderAnimInstance = Cast<UKMAnimInstance>(leaderCharacter->GetMesh()->GetAnimInstance());
	if(!IsValid(leaderAnimInstance))
	{
		return false;
	}

	UKMAnimInstance* followerAnimInstance = Cast<UKMAnimInstance>(followerCharacter->GetMesh()->GetAnimInstance());
	if(!IsValid(followerAnimInstance))
	{
		return false;
	}

	float maxRadius = 0.f;
	float maxHeight = 0.f;

	FollowerMovementData.StartWorldTransform = followerCharacter->GetMesh()->GetComponentToWorld() * followerAnimInstance->GetPairBlendInfo().OffsetTransform;
	followerAnimInstance->SetPairOffsetTransform(FollowerMovementData);

	if (UKMPawnMoverComponent* followerPawnMoverComponent = followerCharacter->GetMoverComponent())
	{
		followerPawnMoverComponent->SetAnimRootMotionTranslationScale(FVector::Zero());
	}

	maxRadius = followerCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleRadius();
	maxHeight = followerCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleHalfHeight();
	
	float prevHeight = leaderCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleHalfHeight();
	
	if (maxRadius > leaderCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleRadius())
	{
		leaderCharacter->GetMovementShapeComponent()->SetCapsuleRadius(maxRadius);
	}
	else
	{
		maxRadius = leaderCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleRadius();
	}
	
	if (maxHeight > leaderCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleHalfHeight())
	{
		leaderCharacter->GetMovementShapeComponent()->SetCapsuleHalfHeight(maxHeight);
	}
	else
	{
		maxHeight = leaderCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleHalfHeight();
	}

	leaderCharacter->ApplyCapsuleFootAlignment(maxHeight - prevHeight);
	FollowerMovementData.HeightOffset = followerCharacter->GetMovementShapeComponent()->GetScaledCapsuleHalfHeight() - leaderCharacter->GetMovementShapeComponent()->GetScaledCapsuleHalfHeight();
	FollowerMovementData.LeaderActor->SetActorLocation(FollowerMovementData.LeaderActor->GetActorLocation() + FVector(0.f, 0.f, maxHeight - prevHeight));
	
	followerCharacter->ApplyCapsuleFootAlignment(maxHeight - followerCharacter->GetMovementShapeComponent()->GetUnscaledCapsuleHalfHeight());
	
	FollowerMovementData.FollowActor->SetActorLocation(FVector(FollowerMovementData.LeaderActor->GetActorLocation().X, FollowerMovementData.LeaderActor->GetActorLocation().Y, FollowerMovementData.FollowActor->GetActorLocation().Z));
	FollowerMovementData.FollowActor->SetActorRotation((FollowerMovementData.LeaderActor->GetActorForwardVector() * -1.f).Rotation());
	FollowerMovementData.LatestRootWorldTransform = followerCharacter->GetMesh()->GetSocketTransform(TEXT("Root"));

	followerAnimInstance->UpdateAnimation(0.f, true);
	followerCharacter->GetMesh()->RefreshBoneTransforms();

	return true;
}

void UKMPawnMoverComponent::StopFollowActor(AActor* followActor, float duration)
{
	if (!FollowerMovementData.IsValid() || FollowerMovementData.LeaderActor != followActor)
	{
		return;
	}

	FollowerMovementData.State = EKMFollowerMovementStateType::Stoped;
	FollowerMovementData.Duration = duration;
	FollowerMovementData.ElipsedTime = 0.f;

	if (AKMCharacter* leaderCharacter = Cast<AKMCharacter>(FollowerMovementData.LeaderActor))
	{
		leaderCharacter->RestoreCapsuleFootAlignment();
	}

	if (AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor))
	{
		followerCharacter->SetActorLocation(FVector(FollowerMovementData.LatestRootWorldTransform.GetLocation().X, FollowerMovementData.LatestRootWorldTransform.GetLocation().Y, followerCharacter->GetActorLocation().Z));
		if (UKMAnimInstance* followerAnimInstance = Cast<UKMAnimInstance>(followerCharacter->GetMesh()->GetAnimInstance()))
		{
			followerAnimInstance->SetStopOffsetTransform();
		}
		
		followerCharacter->RestoreCapsuleFootAlignment();
		
		FollowerMovementData.Reset();
		if (UKMPawnMoverComponent* followerPawnMoverComponent = followerCharacter->GetMoverComponent())
		{
			followerPawnMoverComponent->SetAnimRootMotionTranslationScale(FVector::One());
		}
	}
}

void UKMPawnMoverComponent::MoveFollowProcessing(float deltaTime, int32 iterations)
{
	if (!FollowerMovementData.IsValid())
	{
		return;
	}

	if (FollowerMovementData.State == EKMFollowerMovementStateType::Started)
	{
		if (StartedFollowActor())
		{
			FollowerMovementData.State = EKMFollowerMovementStateType::Paried;
		}
	}
	else if (FollowerMovementData.State == EKMFollowerMovementStateType::Paried)
	{
		FollowerMovementData.FollowActor->SetActorLocation(FVector(FollowerMovementData.LeaderActor->GetActorLocation().X, FollowerMovementData.LeaderActor->GetActorLocation().Y, FollowerMovementData.FollowActor->GetActorLocation().Z));
		if (AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor))
		{
			FollowerMovementData.LatestRootWorldTransform = followerCharacter->GetMesh()->GetSocketTransform(TEXT("Root"));
		}
	}
	
	else if (FollowerMovementData.State == EKMFollowerMovementStateType::Stoped)
	{
		if (AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor))
		{
			followerCharacter->SetActorLocation(FVector(FollowerMovementData.LatestRootWorldTransform.GetLocation().X, FollowerMovementData.LatestRootWorldTransform.GetLocation().Y, followerCharacter->GetActorLocation().Z));
			FollowerMovementData.Reset();
			if (UKMPawnMoverComponent* followerPawnMoverComponent = followerCharacter->GetMoverComponent())
			{
				followerPawnMoverComponent->SetAnimRootMotionTranslationScale(FVector::One());
			}
		}
	}

	FollowerMovementData.ElipsedTime += deltaTime;
}

void UKMPawnMoverComponent::RegisterMoveBlockReflection(AKMCharacter* targetCharacter)
{
	TSharedPtr<FKMBlockReflectionData>* existBlockReflectData = BlockReflections.FindByPredicate([targetCharacter](const TSharedPtr<FKMBlockReflectionData>& blockReflectionData)
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
	}
}

void UKMPawnMoverComponent::UnregisterMoveBlockReflection(AKMCharacter* targetCharacter)
{
	int32 existIndex = BlockReflections.IndexOfByPredicate([targetCharacter](const TSharedPtr<FKMBlockReflectionData>& blockReflectionData)
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
	
	BlockReflections.RemoveAt(existIndex);
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

void UKMPawnMoverComponent::RestoreDefaultVisualTransform()
{
	BaseVisualComponentTransform = DefaultBaseVisualTransform;
}

void UKMPawnMoverComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	MoveFollowProcessing(deltaTime, 1);
}