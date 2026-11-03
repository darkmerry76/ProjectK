#include "KMAnimInstance.h"
#include "BlendSpacePlayerLibrary.h"
#include "Animation/AnimNode_AssetPlayerBase.h"
#include "Camera/KMCameraActorBase.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Component/EMMoverLayer.h"
#include "Component/KMPawnMoverComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "Core/KMGameplayTag.h"
#include "DefaultMovementSet/LayeredMoves/AnimRootMotionLayeredMove.h"
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
			if (!characterInstance->HasGameplayTag(FKMGameplayTagName::State_Down_Tag) && !characterInstance->HasGameplayTag(FKMGameplayTagName::Block_Control_Rotation_Tag))
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
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetSkelMeshComponent()->GetOwner());
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
	PairBlendInfo.bIsEnableBlend = false;
	
	if (!FollowerMovementData.IsValid())
	{
		return;		
	}

	PairBlendInfo.bIsEnableBlend = true;
	float blendAlpha = (FollowerMovementData.Duration < 0.0001f) ? 1.f : FMath::Min(1.f, FollowerMovementData.ElipsedTime / FollowerMovementData.Duration);

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
						FTransform targetTransform = FTransform(finalLocation) * followerCharacter->GetMesh()->GetComponentTransform();
						
						FTransform followFinalWorldTransform;
						followFinalWorldTransform.Blend(FollowerMovementData.StartWorldTransform, targetTransform, blendAlpha);

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
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetOwningActor()))
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
	UKMPawnMoverComponent* pawnMoverComponent = ownerCharacter->GetMoverComponent();
	if (!IsValid(pawnMoverComponent))
	{
		return false;
	}

	return pawnMoverComponent->IsOnGround(); 
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

void UKMAnimInstance::Montage_PlayForRootMotion(UAnimMontage* montage, float playRate, EMontagePlayReturnType returnValueType, float timeToStartMontageAt, bool bStopAllMontages)
{
	if (!IsValid(montage))
	{
		return;
	}

	Montage_Play(montage, playRate, returnValueType, timeToStartMontageAt, bStopAllMontages);
	if (montage->HasRootMotion())
	{
		UKMPawnMoverComponent* pawnMoverComponent = GetOwningActor()->FindComponentByClass<UKMPawnMoverComponent>();
		if (!IsValid(pawnMoverComponent))
		{
			return;
		}
		
		pawnMoverComponent->CancelFeaturesWithTag(Mover_AnimRootMotion_Montage,true);
		
		TSharedPtr<FEMLayeredMove_AnimRootMotion> newAnimRootMotion = MakeShared<FEMLayeredMove_AnimRootMotion>();
		newAnimRootMotion->MixMode = EMoveMixMode::OverrideVelocity;
		newAnimRootMotion->Priority = 1;
		newAnimRootMotion->MontageState.Montage = montage;
		newAnimRootMotion->MontageState.PlayRate = playRate;
		newAnimRootMotion->MontageState.CurrentPosition = 0.f;
		newAnimRootMotion->MontageState.bEnableAutoBlendOut = montage->bEnableAutoBlendOut;
		newAnimRootMotion->MontageState.BlendOutTimeSeconds = montage->BlendOut.GetBlendTime();
		newAnimRootMotion->DurationMs = -1.f;

		pawnMoverComponent->QueueLayeredMove(newAnimRootMotion);
	}
}

float UKMAnimInstance::Montage_PlayWithBlendInForRootMotion(UAnimMontage* montage, const FAlphaBlendArgs& blendIn, float playRate, EMontagePlayReturnType returnValueType, float timeToStartMontageAt, bool bStopAllMontages)
{
	if (!IsValid(montage))
	{
		return 0.f;
	}

	float result = Montage_PlayWithBlendIn(montage, blendIn, playRate, returnValueType, timeToStartMontageAt, bStopAllMontages);
	if (montage->HasRootMotion())
	{
		UKMPawnMoverComponent* pawnMoverComponent = GetOwningActor()->FindComponentByClass<UKMPawnMoverComponent>();
		if (!IsValid(pawnMoverComponent))
		{
			return 0.f;
		}
		
		pawnMoverComponent->CancelFeaturesWithTag(Mover_AnimRootMotion_Montage,true);
		
		TSharedPtr<FEMLayeredMove_AnimRootMotion> newAnimRootMotion = MakeShared<FEMLayeredMove_AnimRootMotion>();
		newAnimRootMotion->Priority = 1;
		newAnimRootMotion->MixMode = EMoveMixMode::AdditiveVelocity;
		newAnimRootMotion->MontageState.Montage = montage;
		newAnimRootMotion->MontageState.PlayRate = playRate;
		newAnimRootMotion->MontageState.CurrentPosition = 0.f;
		newAnimRootMotion->MontageState.bEnableAutoBlendOut = montage->bEnableAutoBlendOut;
		newAnimRootMotion->MontageState.BlendOutTimeSeconds = montage->BlendOut.GetBlendTime();
		newAnimRootMotion->DurationMs = -1.f;

		pawnMoverComponent->QueueLayeredMove(newAnimRootMotion);
	}
	return result;
}

#endif