#include "KMAnimInstance.h"
#include "BlendSpacePlayerLibrary.h"
#include "Animation/AnimNode_AssetPlayerBase.h"
#include "Camera/KMCameraActorBase.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Character/KMCharacter.h"
#include "Component/KMCharacterMovementComponent.h"
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
UKMAnimInstance::UKMAnimInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	NextDirection = CurrentDirection;

	OnMontageStarted.AddDynamic(this, &UKMAnimInstance::OnMontageStarted_Internal);
}

void UKMAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);

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
				if (PrevActorDirection != characterInstance->GetCharacterDirection())
				{
					PrevActorDirection = characterInstance->GetCharacterDirection();
				}
			}
		}
	}
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

void UKMAnimInstance::TickPairBlend(float deltaTime)
{
	if (!PairBlendInfo.bIsEnableBlend)
	{
		return;
	}

	if (PairBlendInfo.EplisedTime > PairBlendInfo.Duration)
	{
		//PairBlendInfo.bIsEnableBlend = false;
	}
	
	ACharacter* ownerCharacter = Cast<ACharacter>(GetSkelMeshComponent()->GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	float alpha = FMath::Clamp(PairBlendInfo.EplisedTime / PairBlendInfo.Duration, 0.f, 1.f);
	
	//const FVector worldPosition = FMath::Lerp(PairBlendInfo.StartWorldTransform.GetLocation(), ownerCharacter->GetActorLocation() + PairBlendInfo.WorldOffset , 0.f);
	const FVector worldPosition = FMath::Lerp(PairBlendInfo.StartWorldTransform.GetLocation(),ownerCharacter->GetActorLocation() + PairBlendInfo.WorldOffset, alpha);

	const FVector worldVector = ownerCharacter->GetActorLocation() - worldPosition;
	PairBlendInfo.FinalWorldPosition = worldVector;
	PairBlendInfo.EplisedTime += deltaTime;
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