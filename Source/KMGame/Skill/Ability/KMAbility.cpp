#include "KMAbility.h"
#include "EMCurveWarpingComponent.h"
#include "EMMartialArtsComponent.h"
#include "KMAbilityBlow.h"
#include "Character/KMCharacter.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "GameObject/KMCharacterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMTargetSubsystem.h"

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

UKMCharacterInstance* UKMAbility::GetOwnerCharacterInstance() const
{
	return GetTypedOuter<UKMCharacterInstance>();
}

AKMCharacter* UKMAbility::GetOwnerCharacter() const
{
	UKMCharacterInstance* characterInstance = GetOwnerCharacterInstance();
	check(IsValid(characterInstance));
	return characterInstance->GetCharacter();
}

UKMCharacterInstance* UKMAbility::GetTargetCharacterInstance() const
{
	if (!LockOnCluster.IsValid())
	{
		return nullptr;
	}
	return LockOnCluster->GetBestTarget();
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

AKMCharacter* UKMAbility::GetTargetCharacter() const
{
	UKMCharacterInstance* targetCharacterInstance = GetTargetCharacterInstance();
	if(!IsValid(targetCharacterInstance))
	{
		return nullptr;
	}

	AKMCharacter* targetCharacter = targetCharacterInstance->GetCharacter();
	check(IsValid(targetCharacter));

	return targetCharacter;
}

void UKMAbility::Activate()
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	UEMMartialArts* martialArts = GetMartialArts();
	check(IsValid(martialArts));

	if (IsValid(BlowCurve))
	{
		UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
		check(IsValid(curveWarping));

		if (bIsClearCurve)
		{
			curveWarping->ClearCurveWarping();
		}
		
		FVector actorForwardVector = ownerCharacter->GetActorForwardVector();
		actorForwardVector.Z = 0.f;
		actorForwardVector.Normalize();

		FVector targetLocation = ownerCharacter->GetActorLocation() + (HorizontalPower * ownerCharacter->GetActorForwardVector() * -1.f);
		
		if (UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement()))
		{
			if (CustomMovementMode != EKMCustomMovementMode::None)
			{
				characterMovement->SetCustomMovementMode(CustomMovementMode);
			}
		}

		if (!curveWarping->GetInteruptDelegate().IsAlreadyBound(this, &UKMAbilityBlow::OnCurveWarpingInterrupt))
		{
			curveWarping->GetInteruptDelegate().AddDynamic(this, &UKMAbilityBlow::OnCurveWarpingInterrupt);
		}
		curveWarping->PlayCurveWarpjng(BlowCurve, targetLocation, Duration, VerticalPower / 100.F, false, false);
	}
	OnActivated();
}

void UKMAbility::OnActivated_Implementation()
{
}

void UKMAbility::Deactivate(bool bCancel)
{
	StopMartialArts();
	OnDeacivated(bCancel);
}

void UKMAbility::OnDeacivated_Implementation(bool bCancel)
{
}

void UKMAbility::OnTriggerEvent_Implementation(const FGameplayTag& eventTag)
{
}

void UKMAbility::OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType type)
{
}

void UKMAbility::PlayMartialArts(TSharedPtr<FEMMartialArtsContextData> newContextData, float newRate, bool bLooping)
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	UEMMartialArtsComponent* martialArtsComponent = ownerCharacter->GetMartialArtsComponent();
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
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	UEMMartialArtsComponent* martialArtsComponent = ownerCharacter->GetMartialArtsComponent();
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

void UKMAbility::Trigger(const FGameplayTag eventTag)
{
}

FAnimMontageInstance* UKMAbility::PlayerMontage(UAnimMontage* montage, float playRate, FName startSectionName)
{
	GetOwnerCharacter()->PlayAnimMontage(montage, playRate, startSectionName);

	if (UAnimInstance* animInstance = GetOwnerCharacter()->GetMesh()->GetAnimInstance())
	{
		return animInstance->GetInstanceForMontage(montage);
	}
	return nullptr;
}

void UKMAbility::StopMontage(UAnimMontage* montage)
{
	GetOwnerCharacter()->StopAnimMontage(montage);
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
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	FVector result = ownerCharacter->GetActorLocation() + (direction * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? ownerCharacter->GetActorLocation().Z : result.Z);	
}

FVector UKMAbility::GetOffsetAlongOwnerForward(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));
	
	return GetOffsetAlongOwnerDirection(ownerCharacter->GetActorForwardVector(), offsetDistance, weight, bIgnoreZ);
}

FVector UKMAbility::GetOffsetAlongTargetForward(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AKMCharacter* targetCharacter = GetTargetCharacter();
	check(IsValid(targetCharacter));

	FVector result = targetCharacter->GetActorLocation() + (targetCharacter->GetActorForwardVector() * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetCharacter->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOffsetToTargetByOwnerForward(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AKMCharacter* targetCharacter = GetTargetCharacter();
	check(IsValid(targetCharacter));

	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	FVector result = targetCharacter->GetActorLocation() + (ownerCharacter->GetActorForwardVector() * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetCharacter->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOffsetOwnerAlongTargetDirection(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	AKMCharacter* targetCharacter = GetTargetCharacter();
	if(!IsValid(targetCharacter))
	{
		return ownerCharacter->GetActorLocation(); 
	}
	
	FVector targetToOwner = targetCharacter->GetActorLocation() - ownerCharacter->GetActorLocation();
	targetToOwner.Normalize();

	FVector result = ownerCharacter->GetActorLocation() + (targetToOwner * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetCharacter->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOffsetTargetAlongOwnerDirection(float offsetDistance, float weight, bool bIgnoreZ) const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	AKMCharacter* targetCharacter = GetTargetCharacter();
	if(!IsValid(targetCharacter))
	{
		return ownerCharacter->GetActorLocation(); 
	}
	
	FVector targetToOwner = ownerCharacter->GetActorLocation() - targetCharacter->GetActorLocation();
	targetToOwner.Normalize();

	FVector result = targetCharacter->GetActorLocation() + (targetToOwner * offsetDistance * weight);
	return FVector(result.X, result.Y, bIgnoreZ ? targetCharacter->GetActorLocation().Z : result.Z);
}

FVector UKMAbility::GetOwnerFootLocation(float offsetHeight) const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	FVector footLocation = ownerCharacter->GetActorLocation();
	footLocation.Z -= ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + offsetHeight;

	return footLocation;
}

void UKMAbility::AddOwnerMotionWarpingLocation(FName targetName, FVector targetLocation)
{
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UEMCurveWarpingComponent* curveWarping = character->GetCurveWarping();
	check(IsValid(curveWarping));

	curveWarping->AddOrUpdateWarpTargetFromLocation(targetName, targetLocation);
}

void UKMAbility::PlayOwnerCurveWarping(UCurveBase* newCurveAsset, FVector newTargetLocation, float newPlayLength, float newZScale, bool bIgnoreZ, bool bAutoEndingWalk)
{
	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UEMCurveWarpingComponent* curveWarping = character->GetCurveWarping();
	check(IsValid(curveWarping));

	curveWarping->PlayCurveWarpjng(newCurveAsset, newTargetLocation, newPlayLength, newZScale, bIgnoreZ, bAutoEndingWalk);
}

UAnimMontage* UKMAbility::GetOwnerAnimationTag(FGameplayTag tag) const
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));
	
	return ownerCharacter->GetAnimationTag(tag);
}

int32 UKMAbility::GetMartialArtsHandle() const
{
	return MartialArtsHandle;
}

void UKMAbility::ForceComplate()
{
}
