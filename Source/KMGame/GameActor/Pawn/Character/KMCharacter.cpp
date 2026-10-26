#include "KMCharacter.h"
#include "Animation/AnimSet/KMAnimationSetTag.h"
#include "Component/KMPawnMoverComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "Component/KMWidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMBeastPDA.h"
#include "DataAsset/KMItemPDA.h"
#include "GameActor/Item/KMItemAppearanceActor.h"
#include "GameObject/KMGameObjectInstance.h"
#include "GameObject/KMHeroInstance.h"
#include "Sound/KMSoundSetTag.h"
#include "Tables/Generated/KMTable_Item.h"

AKMCharacter::AKMCharacter(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SkeletalMeshComponent = CreateDefaultSubobject<UKMSkeletalMeshComponent>(TEXT("Mesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	
	CameraTarget = CreateDefaultSubobject<USceneComponent>(TEXT("CameraTarget"));
	CameraTarget->SetupAttachment(SkeletalMeshComponent);

	WidgetComponent = CreateDefaultSubobject<UKMWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(SkeletalMeshComponent);
}

void AKMCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetController())
	{
		AddTickPrerequisiteActor(GetController());
	}
	
	if (DefaultWeaponItem != NAME_None)
	{
		const FKMTable_ItemRow* itemRow = FKMTable_ItemRow::FindRowPtr(DefaultWeaponItem);
		check(itemRow);

		UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
		check(IsValid(assetManager));
		
		UKMItemPDA* itemPDA = Cast<UKMItemPDA>(assetManager->GetAsset(itemRow->ItemPdaKey));
		check(IsValid(itemPDA));

		AKMItemAppearanceActor* itemActorCDO = itemPDA->ItemClass->GetDefaultObject<AKMItemAppearanceActor>();
		check(IsValid(itemActorCDO));

		WeaponInstance = itemActorCDO->CreateInstance(this, itemPDA->InstanceClass);
		WeaponInstance->Spawn();

		WeaponInstance->Equip(GetMesh(), TEXT("Bip001-L-Hand"));
	}
}

void AKMCharacter::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);

	if (IsValid(WeaponInstance))
	{
		WeaponInstance->Despawn();
	}
}

void AKMCharacter::OnConstruction(const FTransform& transform)
{
	Super::OnConstruction(transform);

	MoverComponent->SetPrimaryVisualComponent(SkeletalMeshComponent);
}

UKMWidgetComponent* AKMCharacter::GetWidgetComponent() const
{
	return WidgetComponent;
}

USceneComponent* AKMCharacter::GetCameraTarget() const
{
	return CameraTarget;
}

UKMSkeletalMeshComponent* AKMCharacter::GetMesh() const
{
	return SkeletalMeshComponent;
}

FTransform AKMCharacter::GetCarryOffsetTransform_Implementation() const
{
	return FTransform::Identity;
}

const FVector& AKMCharacter::GetInputVelocity() const
{
	return InputVelocity;
}

const FVector& AKMCharacter::GetLatestMoveInputVelocity() const
{
	return LastetMoveInputVelocity;
}

const FHitResult AKMCharacter::GetLatestWallHitResult_Implementation() const
{
	return LastWallHitResult;
}

UKMCharacterInstance* AKMCharacter::GetCharacterInstance() const
{
	return Cast<UKMCharacterInstance>(GetGameObjectInstance());
}

UAnimMontage* AKMCharacter::GetAnimationTagOriginal(FGameplayTag tag) const
{
	if (UKMCharacterInstance* characterInstance = GetCharacterInstance())
	{
		if (characterInstance->IsBeast() &&
			IsValid(BeastPDA) &&
			IsValid(BeastPDA->AnimSet))
		{
			return BeastPDA->AnimSet->GetAnimation(tag);
		}
	}
	return AnimsetTag->GetAnimation(tag);
}

void AKMCharacter::LandHit(const FHitResult& hitResult)
{
	Super::LandHit(hitResult);
	MontqagePlayTag(FKMGameplayTagName::Anim_Landing_0);
}

UAnimMontage* AKMCharacter::GetAnimationTag(FGameplayTag tag) const
{
	if (!IsValid(AnimsetTag))
	{
		return nullptr;
	}
	
	const TObjectPtr<UAnimMontage>* existOverrideMontage = AnimOverrideMontageMap.Find(tag);
	if (existOverrideMontage && IsValid(*existOverrideMontage))
	{
		return *existOverrideMontage;
	}
	
	return GetAnimationTagOriginal(tag);
}

float AKMCharacter::MontqagePlay(UAnimMontage* animMontage, float playRate, EMontagePlayReturnType returnValueType, float timeToStartMontageAt, bool bStopAllMontages)
{
	UAnimInstance* animInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
	if (!IsValid(animMontage))
	{
		return 0.f;
	}
	return animInstance->Montage_Play(animMontage, playRate, returnValueType, timeToStartMontageAt, bStopAllMontages);
}

void AKMCharacter::StopAnimMontage(UAnimMontage* animMontage)
{
	UAnimInstance* animInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
	if (!IsValid(animInstance))
	{
		return;
	}

	bool bShouldStopMontage = !animInstance->Montage_GetIsStopped(animMontage);

	if ( bShouldStopMontage )
	{
		animInstance->Montage_Stop(animMontage->BlendOut.GetBlendTime(), animMontage);
	}
}


float AKMCharacter::MontqagePlayTag(FGameplayTag tag, float playRate, EMontagePlayReturnType returnValueType, float timeToStartMontageAt, bool bStopAllMontages)
{
	UAnimMontage* tagAnimMontage = GetAnimationTag(tag);
	
	return MontqagePlay(tagAnimMontage, playRate, returnValueType, timeToStartMontageAt, bStopAllMontages);
}

void AKMCharacter::SetMovementOverrideMontage(UAnimMontage* jumpMontage, UAnimMontage* landingMontage)
{
	//AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Jump_0, jumpMontage);
	//AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Landing_0, landingMontage);

/*	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Jump_1, jumpMontage);
	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Landing_1, landingMontage);

	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Jump_2, jumpMontage);
	AnimOverrideMontageMap.FindOrAdd(FKMGameplayTagName::Anim_Landing_2, landingMontage);*/
}

void AKMCharacter::RevertOverrideAnimationTag(FGameplayTag tag)
{
	TObjectPtr<UAnimMontage>* existOverrideMontage = AnimOverrideMontageMap.Find(tag);
	if (!existOverrideMontage)
	{
		return;
	}

	if (UAnimMontage* originMontage = GetAnimationTagOriginal(tag))
	{
		if (UAnimInstance* animInstance = GetMesh()->GetAnimInstance())
		{
			for (auto montageInstance : animInstance->MontageInstances)
			{
				if (montageInstance != animInstance->GetRootMotionMontageInstance() && animInstance->MontageInstances.Num() > 1)
				{
					continue;
				}
				if (montageInstance->Montage == *existOverrideMontage)
				{
					//animInstance->Montage_Play(originMontage, 1.f, EMontagePlayReturnType::MontageLength, montageInstance->GetPosition());
					break;
				}
			}
		}
	}
	AnimOverrideMontageMap.Remove(tag);
}

void AKMCharacter::RemoveMovementOverrideMontage()
{
	//RevertOverrideAnimationTag(FKMGameplayTagName::Anim_Jump_0);
	//RevertOverrideAnimationTag(FKMGameplayTagName::Anim_Landing_0);
/*	RevertOverrideAnimationTag(FKMGameplayTagName::Anim_Jump_1);
	RevertOverrideAnimationTag(FKMGameplayTagName::Anim_Landing_1);
	RevertOverrideAnimationTag(FKMGameplayTagName::Anim_Jump_2);
	RevertOverrideAnimationTag(FKMGameplayTagName::Anim_Landing_2);*/
}

USoundBase* AKMCharacter::GetSoundTag(FGameplayTag tag) const
{
	if (UKMCharacterInstance* characterInstance = GetCharacterInstance())
	{
		if (characterInstance->IsBeast() && IsValid(BeastPDA) && IsValid(BeastPDA->AnimSet))
		{
			return BeastPDA->SoundSet->GetSound(tag);
		}
	}
	return SoundsetTag->GetSound(tag);
}

UKMItemAppearanceInstance* AKMCharacter::GetWeaponInstance() const
{
	return WeaponInstance;
}

void AKMCharacter::SetBeastPDA(const UKMBeastPDA* newBeastPDA)
{
	BeastPDA = newBeastPDA;
}

const UKMBeastPDA* AKMCharacter::GetBeastPDA() const
{
	return BeastPDA;
}

FTransform AKMCharacter::GetApproachPullPoint(EKMApproachPullPointType approachPullPointType) const
{
	USkeletalMeshComponent* skeletalMeshComponent = GetMesh();
	if (!IsValid(skeletalMeshComponent))
	{
		return GetActorTransform();
	}
	return skeletalMeshComponent->GetSocketTransform(TEXT("PullPointNeckSocket"));
}

void AKMCharacter::StartJump()
{
	MontqagePlayTag(FKMGameplayTagName::Anim_Jump_0);
}

void AKMCharacter::AddMovementInput(FVector worldDirection, float scaleValue, bool bForce)
{
	Super::AddMovementInput(worldDirection, scaleValue, bForce);
}