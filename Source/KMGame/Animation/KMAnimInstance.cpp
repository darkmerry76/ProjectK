#include "KMAnimInstance.h"
#include "BlendSpacePlayerLibrary.h"
#include "Animation/AnimNode_AssetPlayerBase.h"
#include "Camera/KMCameraActorBase.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Core/KMGameplayTag.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMCharacterInstance.h"
#include "Util/KMUtil.h"

FKMAnimInstanceProxy::FKMAnimInstanceProxy(UAnimInstance* instance) : FAnimInstanceProxy(instance)
{
}

void FKMAnimInstanceProxy::PreUpdate(UAnimInstance* animInstance, float deltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(animInstance, deltaSeconds);
	
	const UKMAnimInstance* castAnimInstance = static_cast<const UKMAnimInstance*>(animInstance);
	check(IsValid(castAnimInstance));
	SlotBlendInfo = castAnimInstance->GetSlotBlendInfo();
	ShakeData = castAnimInstance->GetShakeData();
	PairBlendInfo = castAnimInstance->GetPairBlendInfo();
}

const FKMMultiSlotBlendInfo& FKMAnimInstanceProxy::GetSlotBlendInfo() const
{
	return SlotBlendInfo;
}

const FKMAnimNodeShakeData& FKMAnimInstanceProxy::GetShakeData() const
{
	return ShakeData;
}

const FKMPairPositionBlendInfo& FKMAnimInstanceProxy::GetPairBlendInfo() const
{
	return PairBlendInfo;
}

void FKMAnimInstanceProxy::ResetRootNode(bool bInDeferRootNodeInitialization)
{
	InitializeRootNode(bInDeferRootNodeInitialization);
}

UKMAnimInstance::UKMAnimInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAnimInstance::ResetEntryNode()
{
	GetProxyOnGameThread<FKMAnimInstanceProxy>().ResetRootNode();
}

void UKMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	NextDirection = CurrentDirection;

	OnMontageStarted.AddDynamic(this, &UKMAnimInstance::OnMontageStarted_Internal);
}

void UKMAnimInstance::PreUpdateAnimation(float deltaSeconds)
{
	int32 currFrameCount = GFrameCounter;
	TickSlotBlend(deltaSeconds);
	TickShake(GetWorld()->GetDeltaSeconds());
	TickPairBlend(GetWorld()->GetDeltaSeconds());

	if (!IsCustomWalking())
	{
		ResetMovementElipsedTime();
	}
	
	if(AKMCharacter* ownerCharacter = Cast<AKMCharacter>(TryGetPawnOwner()))
	{
		if (UKMCharacterInstance* characterInstance = ownerCharacter->GetCharacterInstance())
		{
			if (!characterInstance->HasGameplayTag(FKMGameplayTagName::State_Blow_Down_Tag) && !characterInstance->HasGameplayTag(FKMGameplayTagName::Block_Control_Rotation_Tag))
			{
				CurrentDirection = UKMUtil::FInterpToCircular(CurrentDirection, NextDirection, deltaSeconds, LerpDirectionSpeed);
				if (PrevActorDirection != characterInstance->GetDirection())
				{
					PrevActorDirection = characterInstance->GetDirection();
				}
			}
		}
	}

	Super::PreUpdateAnimation(deltaSeconds);
}

FAnimInstanceProxy* UKMAnimInstance::CreateAnimInstanceProxy()
{
	return new FKMAnimInstanceProxy(this);
}

#if WITH_EDITOR

void UKMAnimInstance::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	Super::PostEditChangeProperty(propertyChangedEvent);

	if (propertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimInstance, CurrentDirection))
	{
		NextDirection = CurrentDirection;
	}
}

void UKMAnimInstance::SetCurrentDirection(float newCurrentDirection)
{
	CurrentDirection = newCurrentDirection;	
}

float UKMAnimInstance::GetCurrentDirection() const
{
	return CurrentDirection;
}

void UKMAnimInstance::SetNextDirection(float newNextDirection)
{
	NextDirection = newNextDirection;
}

float UKMAnimInstance::GetNextDirection() const
{
	return NextDirection;
}

void UKMAnimInstance::BlendPairPosition(const FTransform& startWorldTransform, const FVector& targetWorldOffset, float newDuration)
{
	ACharacter* ownerCharacter = Cast<ACharacter>(GetSkelMeshComponent()->GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	PairBlendInfo.bIsEnableBlend = true;
	PairBlendInfo.StartWorldTransform = startWorldTransform;

	PairBlendInfo.WorldOffset = targetWorldOffset;
	PairBlendInfo.Duration = newDuration;
	PairBlendInfo.EplisedTime = 0.f;
	PairBlendInfo.FinalWorldPosition = ownerCharacter->GetActorLocation() - PairBlendInfo.StartWorldTransform.GetLocation();
}

void UKMAnimInstance::SetPairOffsetTransform(const FKMFollowerMovementData& followerMovementData)
{
	//PairBlendInfo.bIsEnableBlend = true;
	//PairBlendInfo.OffsetTransform = newPairOffsetTransform;

	FollowerMovementData = followerMovementData;
}

void UKMAnimInstance::SetStopOffsetTransform()
{
	FollowerMovementData.LeaderActor = nullptr;
	FollowerMovementData.FollowActor = nullptr;
	PairBlendInfo.OffsetTransform = FTransform::Identity;
}

void UKMAnimInstance::TickPairBlend(float deltaTime)
{
	if (!FollowerMovementData.IsValid())
	{
		return;		
	}

	float blendAlpha = (FollowerMovementData.Duration < 0.0001f) ? 1.f : (FollowerMovementData.ElipsedTime / FollowerMovementData.Duration);

	PairBlendInfo.OffsetTransform = FTransform::Identity;
	if (AKMCharacter* leaderCharacter = Cast<AKMCharacter>(FollowerMovementData.LeaderActor))
	{
		if (const FAnimMontageInstance* laaderMontageInsance = UKMUtil::FindMontageInstaceTagByCharacter(leaderCharacter, FollowerMovementData.LeaderMontageInstanceId))
		{
			if (const AKMCharacter* followerCharacter = Cast<AKMCharacter>(FollowerMovementData.FollowActor))
			{
				if (const FAnimMontageInstance* followMontageInsance = UKMUtil::FindMontageInstaceTagByCharacter(followerCharacter, FollowerMovementData.FollowMontageInstanceId))
				{
					FTransform leaderBoneTransform;
					FTransform followerBoneTransform;

					const FName boneName = TEXT("Root");

					float position = laaderMontageInsance->GetPosition();

					UKMUtil::GetMontageBoneCSTransform(boneName, laaderMontageInsance->Montage, position, leaderBoneTransform);
					UKMUtil::GetMontageBoneCSTransform(boneName, followMontageInsance->Montage, position, followerBoneTransform);

					FVector finalLocation = (leaderBoneTransform.GetLocation()) - (followerBoneTransform.GetLocation() * -1.f);
					finalLocation.Z = 0.f;
					finalLocation += FollowerMovementData.OffsetTransform.GetLocation();

					if (blendAlpha < 1.f)
					{
						FTransform followFinalWorldTransform;
						followFinalWorldTransform.Blend(FollowerMovementData.StartWorldTransform, followerCharacter->GetMesh()->GetComponentTransform() * FTransform(finalLocation), blendAlpha);

						finalLocation = (followFinalWorldTransform * followerCharacter->GetMesh()->GetComponentTransform().Inverse()).GetLocation();
					}

					FTransform finalOffsetTransform = FTransform::Identity;
					finalOffsetTransform.SetLocation(finalLocation);

					PairBlendInfo.OffsetTransform = finalOffsetTransform;
				}
			}
		}
	}

	FollowerMovementData.ElipsedTime += deltaTime;
}

FTransform UKMAnimInstance::GetPairBlendWorldTransform() const
{
	if (ACharacter* ownerCharacter = Cast<ACharacter>(GetOwningActor()))
	{
		return ownerCharacter->GetMesh()->GetSocketTransform(TEXT("Root"));
	}
	
	return GetOwningActor()->GetActorTransform();
}

void UKMAnimInstance::BlendSlot(EKMAnimSlotType newSlotType, float newWeight, float blendTime)
{
	if (!bIsSlotBlending)
	{
		return;
	}

	FName newSlotName = UKMUtil::GetAnimSlotName(newSlotType);
	NextSlotBlendInfo.BlendWeight = newWeight;
	NextSlotBlendInfo.TargetSlot = newSlotName;
	SlotBlendTime = blendTime;
	if (blendTime < ZERO_ANIMWEIGHT_THRESH)
	{
		SlotBlendInfo = NextSlotBlendInfo;
		return;
	}
	
	const float distance = FMath::Abs(newWeight - SlotBlendInfo.BlendWeight);
	SlotBlendElapsedTime = 0.f;
	SlotBlendTime = blendTime * distance;
	StartBlendWeight = SlotBlendInfo.BlendWeight;
}

void UKMAnimInstance::StartShake(float newDistance, float newFrequency, float newDuration)
{
	ShakeData.bIsEnable = true;
	ShakeData.Distance = newDistance;
	ShakeData.Frequency = newFrequency;
	ShakeData.Duration = newDuration;
	ShakeData.ElapsedTime = 0.f;

	if (AKMPlayerCameraManager* cameraManager = AKMPlayerCameraManager::GetActiveCameraManager(this))
	{
		if (IsValid(cameraManager->GetCurrentCamera()))
		{
			ShakeData.CameraUp = cameraManager->GetCurrentCamera()->GetActorUpVector();
			ShakeData.CameraRight = cameraManager->GetCurrentCamera()->GetActorRightVector();
		}
	}
}

void UKMAnimInstance::TickSlotBlend(float deltaTime)
{
	if (!bIsSlotBlending)
	{
		return;
	}
	if (FMath::IsNearlyEqual(SlotBlendInfo.BlendWeight, NextSlotBlendInfo.BlendWeight))
	{
		return;
	}
	float alpha = SlotBlendElapsedTime / SlotBlendTime;
	if (alpha >= 1.f)
	{
		SlotBlendInfo = NextSlotBlendInfo;
		return;
	}
	
	alpha = FMath::Clamp(alpha, 0.f, 1.f);

	SlotBlendInfo.BlendWeight = FMath::Lerp(StartBlendWeight, NextSlotBlendInfo.BlendWeight, alpha);
	SlotBlendElapsedTime += deltaTime;
}

void UKMAnimInstance::TickShake(float deltaTime)
{
	if (!ShakeData.bIsEnable)
	{
		return;
	}
	if (ShakeData.ElapsedTime > ShakeData.Duration)
	{
		ShakeData.bIsEnable = false;
		return;
	}
	ShakeData.ElapsedTime += deltaTime;
}

const TArray<FName>& UKMAnimInstance::GetHiddenBones() const
{
	return HiddenBones;
}

float UKMAnimInstance::GetMovementElapsedTime() const
{
	return MovementElipsedTime;
}

void UKMAnimInstance::ResetMovementElipsedTime()
{
	MovementElipsedTime = 0.f;
}

void UKMAnimInstance::InitOnMoveBlendSpaceUpdate(const FAnimUpdateContext& context, const FAnimNodeReference& node)
{
	ResetMovementElipsedTime();
}

void UKMAnimInstance::OnMoveBlendSpaceUpdate(const FAnimUpdateContext& context, const FAnimNodeReference& node)
{
	MovementElipsedTime = node.GetAnimNode<FAnimNode_AssetPlayerBase>().GetAccumulatedTime();
}

bool UKMAnimInstance::IsCustomWalking() const
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(TryGetPawnOwner());
	if (!IsValid(ownerCharacter))
	{
		return false;
	}
	UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
	if (!IsValid(characterMovement))
	{
		return false;
	}

	return characterMovement->MovementMode == EMovementMode::MOVE_Walking; 
}

const FKMMultiSlotBlendInfo& UKMAnimInstance::GetSlotBlendInfo() const
{
	return SlotBlendInfo;
}

const FKMMultiSlotBlendInfo& UKMAnimInstance::GetNextSlotBlendInfo() const
{
	return NextSlotBlendInfo;
}

const FKMAnimNodeShakeData& UKMAnimInstance::GetShakeData() const
{
	return ShakeData;
}

const FKMPairPositionBlendInfo& UKMAnimInstance::GetPairBlendInfo() const
{
	return PairBlendInfo;
}

int32 UKMAnimInstance::GetLastPlayedMontageInstanceId() const
{
	return LastPlayedMontageInstanceId;
}

void UKMAnimInstance::SetMontageInstanceTag(int32 instanceId, const FName& newTag)
{
	if (instanceId == INDEX_NONE)
	{
		return;
	}
	TagByMontageInstanceIds.Add(newTag, instanceId);
}

int32 UKMAnimInstance::GetMontageInstanceIdByTag(const FName& tag) const
{
	const int32* exist = TagByMontageInstanceIds.Find(tag);
	if (!exist)
	{
		return INDEX_NONE;
	}
	return *exist;
}

void UKMAnimInstance::OnMontageStarted_Internal(UAnimMontage* montage)
{
	LastPlayedMontageInstanceId = INDEX_NONE;
	if (!MontageInstances.IsEmpty())
	{
		if (MontageInstances.Last()->IsValid())
		{
			LastPlayedMontageInstanceId = MontageInstances.Last()->GetInstanceID();
		}
	}
}

void UKMAnimInstance::OnMontageInstanceStopped(FAnimMontageInstance& stoppedMontageInstance)
{
	for (auto intanceItr = TagByMontageInstanceIds.CreateIterator(); intanceItr; ++intanceItr)
	{
		if (intanceItr.Value() == stoppedMontageInstance.GetInstanceID())
		{
			intanceItr.RemoveCurrent();
			break;
		}
	}
}

#endif	