#include "KMAbility.h"
#include "EMCurveWarpingComponent.h"
#include "EMMartialArtsComponent.h"
#include "Character/KMCharacter.h"
#include "Component/KMMartialArtsComponent.h"
#include "GameObject/KMCharacterInstance.h"
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
	UKMCharacterInstance* characterInstance = GetOwnerCharacterInstance();
	check(IsValid(characterInstance));

	OnActivated();
}

void UKMAbility::OnActivated_Implementation()
{
}

void UKMAbility::Deactivate()
{
	StopMartialArts();
	OnDeacivated();
}

void UKMAbility::OnDeacivated_Implementation()
{
}

void UKMAbility::OnTriggerEvent_Implementation(const FGameplayTag& eventTag)
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
	if (SkillInstance.IsValid())
	{
		SkillInstance.Pin()->SkillEffectTriggerDelegate.Broadcast(eventTag, SkillInstance.Pin());
	}
}

void UKMAbility::SetSkillInstance(const TSharedPtr<FKMSkillInstance> newSkillInstance)
{
	SkillInstance = newSkillInstance;
	if (SkillInstance.IsValid())
	{
		SkillInstance.Pin()->SetAbility(this);
	}
}

FKMSkillInstance* UKMAbility::GetSkillInstance() const
{
	return SkillInstance.IsValid() ? SkillInstance.Pin().Get() : nullptr;
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

UKMAnimationSetTag* UKMAbility::GetOwnerAnimationSetTag() const
{
	UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
	if (!IsValid(ownerCharacterInstance))
	{
		return nullptr;
	}

	return ownerCharacterInstance->GetAnimsetTag();
}