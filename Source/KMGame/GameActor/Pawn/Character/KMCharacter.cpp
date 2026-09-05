#include "KMCharacter.h"
#include "Animation/AnimSet/KMAnimationSetTag.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMBeastPDA.h"
#include "DataAsset/KMItemPDA.h"
#include "GameActor/Item/KMItemAppearanceActor.h"
#include "GameObject/KMGameObjectInstance.h"
#include "GameObject/KMHeroInstance.h"
#include "Skill/KMSkillHandler.h"
#include "Sound/KMSoundSetTag.h"
#include "Tables/Generated/KMTable_Item.h"
#include "Tables/Generated/KMTable_SkillEffect.h"

AKMCharacter::AKMCharacter(const FObjectInitializer& objectInitializer) :
	Super(objectInitializer.SetDefaultSubobjectClass<UKMSkeletalMeshComponent>(ACharacter::MeshComponentName).
		SetDefaultSubobjectClass<UKMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CurveWarpingComponent  = CreateDefaultSubobject<UKMCurveWarpingComponent>(TEXT("CurveWarping"));
	MartialArtsComponent = CreateDefaultSubobject<UKMMartialArtsComponent>(TEXT("MartialArts"));
	if (UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(GetCharacterMovement()))
	{
		characterMovement->CustomMovementDelegate.AddUObject(CurveWarpingComponent, &UEMCurveWarpingComponent::OnCustomMovement);
	}

	GetMesh()->SetCustomDepthStencilValue(1);
	GetMesh()->SetRenderCustomDepth(true);
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

void AKMCharacter::Landed(const FHitResult& hitResult)
{
	Super::Landed(hitResult);
	
	if (UKMCharacterInstance* characterInstance = GetCharacterInstance())
	{
		if (UKMSkillHandler* skillHandler = characterInstance->GetSkillHandler())
		{
			if (GFrameCounter - LasteLandedFrameCount > 1)
			{
				MontqagePlayTag(FKMGameplayTagName::Anim_Landing_0);
				skillHandler->TriggerEvent(FKMGameplayTagName::Event_Move_Landing_Tag);
				LasteLandedFrameCount = GFrameCounter;
			}
		}
	}
}

void AKMCharacter::OnImpact(const TSharedPtr<FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	check(skillEffectInstance.IsValid() && skillEffectInstance->GetEffectTableRecord());
	Receive_OnImpact(skillEffectInstance->GetEffectTableRecord()->Id, hitClosestPoint, hitTag);	
}

void AKMCharacter::OnDeath()
{
	Receive_OnDeath();
}

UPrimitiveComponent* AKMCharacter::GetMovementShapeComponent() const
{
	return ACharacter::GetCapsuleComponent();
}

UKMMartialArtsComponent* AKMCharacter::GetMartialArtsComponent() const
{
	return MartialArtsComponent;
}

UKMCurveWarpingComponent* AKMCharacter::GetCurveWarpingComponent() const
{
	return CurveWarpingComponent;
}


UMeshComponent* AKMCharacter::GetMasterMeshComponent() const
{
	return GetMesh();
}

UKMAttachedBlendingComponent* AKMCharacter::GetAttachedBlendingComponent() const
{
	return nullptr;
}

FTransform AKMCharacter::GetCarryOffsetTransform() const
{
	return FTransform::Identity;
}

void AKMCharacter::SetMirror(bool bMirror)
{
	bIsMirror = bMirror;
}

bool AKMCharacter::GetMirror() const
{
	return bIsMirror;
}

const FVector& AKMCharacter::GetInputVelocity() const
{
	return InputVelocity;
}

const FVector& AKMCharacter::GetLatestMoveInputVelocity() const
{
	return LastetMoveInputVelocity;
}

UKMCharacterInstance* AKMCharacter::GetCharacterInstance() const
{
	return Cast<UKMCharacterInstance>(GetGameObjectInstance());
}

UKMGameObjectInstance* AKMCharacter::GetGameObjectInstance() const
{
	if (!CharacterInstance.IsValid())
	{
		return nullptr;
	}
	return CharacterInstance.Get();
}

void AKMCharacter::PossessedByGameObjectInstance(UKMGameObjectInstance* newGameObjectInstance)
{
	CharacterInstance = Cast<UKMCharacterInstance>(newGameObjectInstance);
	check(CharacterInstance.IsValid());
	CharacterInstance->SetOwnerActor(this);
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

bool AKMCharacter::IsAir() const
{
	if (UKMCharacterMovementComponent* characterMovementComponent = Cast<UKMCharacterMovementComponent>(GetCharacterMovement()))
	{
		return characterMovementComponent->IsAir();
	}
	return false;
}

void AKMCharacter::StartCrarry(UKMGameObjectInstance* carriedGameObjectInstance)
{
	Receive_OnStartCarry(carriedGameObjectInstance);
}

void AKMCharacter::StartCrarried(UKMGameObjectInstance* carryGameObjectInstance)
{
	Receive_OnStartCarried(carryGameObjectInstance);
}

void AKMCharacter::ComplatePutdown(UKMGameObjectInstance* putDownedGameObjectInstance)
{
	Receive_OnComplatePutdown(putDownedGameObjectInstance);
}

void AKMCharacter::ComplatePutdowned(UKMGameObjectInstance* putDownGameObjectInstance)
{
	Receive_OnComplatePutdowned(putDownGameObjectInstance);
}