#include "KMAbility.h"
#include "KMAbilityBlow.h"
#include "Animation/KMAnimInstance.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMCharacterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "System/EMMontageCacheManager.h"
#include "System/KMTargetSubsystem.h"
#include "Util/KMUtil.h"

UKMAbility::UKMAbility(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

FPrimaryAssetId UKMAbility::GetPrimaryAssetId() const
{
	const UClass* classPtr = GetClass();
	if (classPtr == nullptr)
	{
		return Super::GetPrimaryAssetId();
	}

	FString assetName = classPtr->GetName();
	assetName.RemoveFromEnd(TEXT("_C"));

	return FPrimaryAssetId("KMAbility", FName(*assetName));
}

UKMGameObjectInstance* UKMAbility::GetOwnerGameObjectInstance() const
{
	return GetTypedOuter<UKMGameObjectInstance>();
}

UKMCharacterInstance* UKMAbility::GetOwnerCharacterInstance() const
{
	return Cast<UKMCharacterInstance>(GetOwnerGameObjectInstance());
}

AActor* UKMAbility::GetOwnerActor() const
{
	UKMGameObjectInstance* gameGameObjectInstance = GetOwnerGameObjectInstance();
	if(!IsValid(gameGameObjectInstance))
	{
		return nullptr;
	}
	
	return gameGameObjectInstance->GetOwnerActor();
}

AKMCharacter* UKMAbility::GetOwnerCharacter() const
{
	return Cast<AKMCharacter>(GetOwnerActor());
}

UKMGameObjectInstance* UKMAbility::GetTargetGameObjectInstance() const
{
	if (!LockOnCluster.IsValid())
	{
		return nullptr;
	}
	return LockOnCluster->GetBestTarget();
}

UKMCharacterInstance* UKMAbility::GetTargetCharacterInstance() const
{
	return Cast<UKMCharacterInstance>(GetTargetGameObjectInstance());
}

UEMMartialArtsComponent* UKMAbility::GetMartialArtsComponent() const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return nullptr;
	}
	return ownerCharacter->GetMartialArtsComponent();
}

AActor* UKMAbility::GetTargetActor() const
{
	UKMGameObjectInstance* targetGameObjectInstance = GetTargetGameObjectInstance();
	if(!IsValid(targetGameObjectInstance))
	{
		return nullptr;
	}
	return targetGameObjectInstance->GetOwnerActor();
}

AKMCharacter* UKMAbility::GetTargetCharacter() const
{
	return Cast<AKMCharacter>(GetTargetActor());
}

void UKMAbility::Activate()
{
	UEMMartialArts* martialArts = GetMartialArts();
	check(IsValid(martialArts));

	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));

	if(IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(ownerActor))
	{
		if (UKMCurveWarpingComponent* curveWarping = pawnInterface->GetCurveWarpingComponent())
		{
			if (IsValid(BlowCurve))
			{
				FVector actorForwardVector = ownerActor->GetActorForwardVector();
				actorForwardVector.Z = 0.f;
				actorForwardVector.Normalize();

				FVector targetLocation = ownerActor->GetActorLocation() + (HorizontalPower * ownerActor->GetActorForwardVector() * -1.f);
				
				if (!curveWarping->GetInteruptDelegate().IsAlreadyBound(this, &UKMAbilityBlow::OnCurveWarpingInterrupt))
				{
					curveWarping->GetInteruptDelegate().AddDynamic(this, &UKMAbilityBlow::OnCurveWarpingInterrupt);
				}
				CurveWapingInstanceId = curveWarping->PlayCurveWarping(CustomMovementMode, BlowCurve, targetLocation, Duration, VerticalPower / 100.F, false);
			}

			if (!Impulse.IsNearlyZero())
			{
				FVector impulseTargetLocation = ownerActor->GetActorLocation() + (ownerActor->GetActorForwardVector() * Impulse.X) + (ownerActor->GetActorRightVector() * Impulse.Y);
				ImpulseInstanceId = curveWarping->PlayLinearWarp(impulseTargetLocation, ImpulseDuration);
			}
		}
	}
	OnActivated();
}

void UKMAbility::OnActivated_Implementation()
{
}

void UKMAbility::Deactivate(bool bCancel)
{
	UEMMartialArts* martialArts = GetMartialArts();
	check(IsValid(martialArts));

	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));

	if(IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(ownerActor))
	{
		UKMCurveWarpingComponent* curveWarping = pawnInterface->GetCurveWarpingComponent();
		if (bIsClearCurve && IsValid(curveWarping))
		{
			if (CurveWapingInstanceId != INDEX_NONE)
			{
				curveWarping->ClearCurveWarping(CustomMovementMode, CurveWapingInstanceId);
			}

			if (ImpulseInstanceId != INDEX_NONE)
			{
				curveWarping->StopLinearWarp(ImpulseInstanceId);
			}
		}
	}
	StopMartialArts();
	OnDeacivated(bCancel);
}

void UKMAbility::OnDeacivated_Implementation(bool bCancel)
{
}

void UKMAbility::OnTriggerEvent_Implementation(const FGameplayTag& eventTag)
{
}

void UKMAbility::OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, float deltaTime, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType interuptType, EEMCustomMovementMode movementMode)
{
}

void UKMAbility::PlayMartialArts(TSharedPtr<UE::Anim::IAnimNotifyEventContextDataInterface> newContextData, float newRate, bool bLooping)
{
	IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(GetOwnerActor());
	if (!pawnInterface)
	{
		return;
	}
	
	UKMMartialArtsComponent* martialArtsComponent = pawnInterface->GetMartialArtsComponent();
	if (!IsValid(martialArtsComponent))
	{
		return;
	}
	
	UEMMartialArts* martialArts = GetMartialArts();
	if (!IsValid(martialArts))
	{
		return;
	}

	if (!newContextData.IsValid())
	{
		TSharedPtr<FKMMartialArtsSkillContextData> newSkillContextData = MakeShared<FKMMartialArtsSkillContextData>(this);
		newContextData = newSkillContextData;
	}

	MartialArtsHandle = martialArtsComponent->PlayEx(martialArts, newContextData, newRate, bLooping);

	TSharedPtr<FEMMartialArtsInstance> martialArtsInstance = martialArtsComponent->GetInstance(MartialArtsHandle);
	if (martialArtsInstance.IsValid())
	{
		martialArtsInstance->EndPlayDelegate.AddUObject(this, &ThisClass::OnMartialArtsEnd);
	}
}

void UKMAbility::OnMartialArtsEnd(FEMMartialArtsInstance* martialArtsInstance)
{
	if (martialArtsInstance)
	{
		K2_OnMartialArtsEnd(martialArtsInstance->GetId());
	}
	else
	{
		K2_OnMartialArtsEnd(INDEX_NONE);
	}
}

void UKMAbility::K2_OnMartialArtsEnd_Implementation(int instanceId)
{
}

void UKMAbility::StopMartialArts()
{
	IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(GetOwnerActor());
	if (!pawnInterface)
	{
		return;
	}
	
	UKMMartialArtsComponent* martialArtsComponent = pawnInterface->GetMartialArtsComponent();
	if (!IsValid(martialArtsComponent))
	{
		return;
	}
	
	UEMMartialArts* martialArts = GetMartialArts();
	if (!IsValid(martialArts))
	{
		return;
	}
	
	martialArtsComponent->StopByInstanceId(MartialArtsHandle);
	
	MartialArtsHandle = INDEX_NONE;
}

void UKMAbility::MontageJump(FName sectionName)
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	USkeletalMeshComponent* skeletalMeshComponent = ownerCharacter->GetMesh();
	if (!IsValid(skeletalMeshComponent))
	{
		return;
	}
	
	UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();
	if (!IsValid(animInstance))
	{
		return;	
	}

	if (!animInstance->Montage_IsActive(Montage))
	{
		return;
	}
	
	animInstance->Montage_JumpToSection(sectionName, Montage);
}

void UKMAbility::OnRequestLoop_Implementation()
{
	MontageJump(TEXT("loop"));
}

void UKMAbility::OnRequestEnd_Implementation()
{
	MontageJump(TEXT("end"));
}

UWorld* UKMAbility::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject) && ensureMsgf(GetOuter(), TEXT("UKMAbility: %s has a null OuterPrivate in UKMAbility::GetWorld()"), *GetFullName())
		&& !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		return Super::GetWorld();
	}
	return nullptr;
}

void UKMAbility::Trigger(const FGameplayTag& eventTag)
{
	OnTriggerEvent(eventTag);
}

FAnimMontageInstance* UKMAbility::PlayMontage(UAnimMontage* montage, float playRate, FName startSectionName, EKMAnimSlotType slotType, float slotBlendTime)
{
	UAnimMontage* usedMontage = FEMMontageCacheManager::Get().GetMontageBySlot(montage, UKMUtil::GetAnimSlotName(slotType));
	if (!IsValid(usedMontage))
	{
		return nullptr;
	}

	if (UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(GetOwnerCharacter()->GetMesh()->GetAnimInstance()))
	{
		animInstance->Montage_Play(usedMontage, playRate);
		if (slotType == EKMAnimSlotType::DefaultSlot)
		{
			animInstance->BlendSlot(slotType, 0.f, slotBlendTime);
		}
		else if (slotType == EKMAnimSlotType::OverrideSlot)
		{
			animInstance->BlendSlot(slotType, 1.f, slotBlendTime);
		}
		return animInstance->GetMontageInstanceForID(animInstance->GetLastPlayedMontageInstanceId());
	}
	return nullptr;
}

void UKMAbility::StopMontage(UAnimMontage* montage)
{
	GetOwnerCharacter()->StopAnimMontage(montage);
}

FAnimMontageInstance* UKMAbility::GetMontageInstanceByTag(AKMCharacter* character, const FName& tag) const
{
	if (!IsValid(character))
	{
		return nullptr;
	}
	
	USkeletalMeshComponent* skeletalMeshComponent = character->GetMesh();
	if (!IsValid(skeletalMeshComponent))
	{
		return nullptr;
	}

	UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(skeletalMeshComponent->GetAnimInstance());
	if (!IsValid(animInstance))
	{
		return nullptr;
	}

	int32 montageId = animInstance->GetMontageInstanceIdByTag(tag);
	if (montageId == INDEX_NONE)
	{
		return nullptr;
	}
	
	return animInstance->GetMontageInstanceForID(montageId);
}

bool UKMAbility::SetMontageRateByTag(AKMCharacter* character, FName tag, float newRate)
{
	FAnimMontageInstance* montageInstance = GetMontageInstanceByTag(character, tag);
	if (!montageInstance)
	{
		return false;
	}
	
	montageInstance->SetPlayRate(newRate);
	return true;
}

float UKMAbility::GetMontageRateByTag(AKMCharacter* character, FName tag) const
{
	const FAnimMontageInstance* montageInstance = GetMontageInstanceByTag(character, tag);
	if (!montageInstance)
	{
		return 1.f;
	}
	
	return montageInstance->GetPlayRate();
}

void UKMAbility::SetLockOnCluster(TSharedPtr<FKMLockOnCluster> newLockOnCluster)
{
	LockOnCluster = newLockOnCluster;
}

void UKMAbility::SetMontageRate(float newRate)
{
	if (MontageInstance)
	{
		Rate = newRate;
		MontageInstance->SetPlayRate(newRate);
	}
}

float UKMAbility::GetMontageRate() const
{
	if (MontageInstance)
	{
		return MontageInstance->GetPlayRate();
	}
	return Rate;
}

void UKMAbility::Impact(const FTransform& newImpactTransform)
{
	ImpactTransform = newImpactTransform;
	
	OnImpact(newImpactTransform);
}

void UKMAbility::OnImpact_Implementation(const FTransform& newImpactTransform)
{
}

FVector UKMAbility::GetOffsetAlongOwnerDirection(const FVector direction, float offsetDistance, float weight, bool bIgnoreZ) const
{
	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));

	FVector result = ownerActor->GetActorLocation() + (direction * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? ownerActor->GetActorLocation().Z : result.Z);	
}

FVector UKMAbility::GetOffsetAlongOwnerForward(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));
	
	return GetOffsetAlongOwnerDirection(ownerActor->GetActorForwardVector(), offsetDistance, weight, bIgnoreZ);
}

FVector UKMAbility::GetOffsetAlongTargetForward(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AActor* targetActor = GetTargetActor();
	check(IsValid(targetActor));

	FVector result = targetActor->GetActorLocation() + (targetActor->GetActorForwardVector() * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetActor->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOffsetToTargetByOwnerForward(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AActor* targetActor = GetTargetActor();
	check(IsValid(targetActor));

	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));

	FVector result = targetActor->GetActorLocation() + (ownerActor->GetActorForwardVector() * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetActor->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOffsetToTargetByOwnerForward2D(FVector2D offset, float weight, bool bIgnoreZ) const
{
	AActor* targetActor = GetTargetActor();
	check(IsValid(targetActor));

	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));

	FVector result = targetActor->GetActorLocation() + (ownerActor->GetActorForwardVector() * offset.X * weight) + (ownerActor->GetActorRightVector() * offset.Y * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetActor->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOffsetOwnerAlongTargetDirection(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));

	AActor* targetActor = GetTargetActor();
	if(!IsValid(targetActor))
	{
		return ownerActor->GetActorLocation(); 
	}
	
	FVector targetToOwner = targetActor->GetActorLocation() - ownerActor->GetActorLocation();
	targetToOwner.Normalize();

	FVector result = ownerActor->GetActorLocation() + (targetToOwner * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetActor->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOffsetTargetAlongOwnerDirection(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));

	AActor* targetActor = GetTargetActor();
	if(!IsValid(targetActor))
	{
		return ownerActor->GetActorLocation(); 
	}
	
	FVector targetToOwner = ownerActor->GetActorLocation() - targetActor->GetActorLocation();
	targetToOwner.Normalize();

	FVector result = targetActor->GetActorLocation() + (targetToOwner * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetActor->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOwnerFootLocation(float offsetHeight) const
{
	AActor* ownerActor = GetOwnerActor();
	check(IsValid(ownerActor));
	
	FVector footLocation = ownerActor->GetActorLocation();
	if (ACharacter* ownerCharacter = Cast<ACharacter>(ownerActor))
	{
		footLocation.Z -= ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + offsetHeight;
	}

	return footLocation;
}

void UKMAbility::AddOwnerMotionWarpingLocation(FName targetName, FVector targetLocation)
{
	AKMCharacter* ownerActor = GetOwnerCharacter();
	if(!IsValid(ownerActor))
	{
		return;
	}

	UKMCurveWarpingComponent* curveWarping = ownerActor->GetCurveWarpingComponent();
	check(IsValid(curveWarping));

	curveWarping->AddOrUpdateWarpTargetFromLocation(targetName, targetLocation);
}

void UKMAbility::PlayOwnerCurveWarping(EEMCustomMovementMode movementMode, UCurveBase* newCurveAsset, FVector newTargetLocation, float newPlayLength, float newZScale, bool bIgnoreZ)
{
	AKMCharacter* character = GetOwnerCharacter();
	if(!IsValid(character))
	{
		return;
	}

	UKMCurveWarpingComponent* curveWarping = character->GetCurveWarpingComponent();
	check(IsValid(curveWarping));

	curveWarping->PlayCurveWarping(movementMode, newCurveAsset, newTargetLocation, newPlayLength, newZScale, bIgnoreZ);
}

void UKMAbility::PlayOwnerLinearWarping(FVector newTargetLocation, float newPlayLength)
{
	AKMCharacter* character = GetOwnerCharacter();
	if(!IsValid(character))
	{
		return;
	}

	UKMCurveWarpingComponent* curveWarping = character->GetCurveWarpingComponent();
	check(IsValid(curveWarping));

	curveWarping->PlayLinearWarp(newTargetLocation, newPlayLength);
}

UAnimMontage* UKMAbility::GetOwnerAnimationTag(FGameplayTag tag) const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if(!IsValid(ownerCharacter))
	{
		return nullptr;
	}
	
	return ownerCharacter->GetAnimationTag(tag);
}

int32 UKMAbility::GetMartialArtsHandle() const
{
	return MartialArtsHandle;
}

void UKMAbility::ForceComplate()
{
}

void UKMAbility::InverseDirection(bool bForce)
{
	UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance();
	if (!IsValid(ownerGameObjectInstance))
	{
		return;
	}
	ownerGameObjectInstance->SetDirection(UKMUtil::InverseCircularDirection(ownerGameObjectInstance->GetDirection()), bForce);
}

void UKMAbility::MontageStart(UAnimMontage* animMontage, FName montageInstanceTag)
{
	OnMontageStart(animMontage, montageInstanceTag);
}

void UKMAbility::MontageStop(class UAnimMontage* animMontage, FName montageInstanceTag)
{
	OnMontageStop(animMontage, montageInstanceTag);
}