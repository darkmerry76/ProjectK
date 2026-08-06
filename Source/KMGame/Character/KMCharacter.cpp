#include "KMCharacter.h"
#include "Actor/KMItemAppearanceActor.h"
#include "Animation/AnimSet/KMAnimationSetTag.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMBeastPDA.h"
#include "DataAsset/KMItemPDA.h"
#include "GameObject/KMGameObjectInstance.h"
#include "GameObject/KMHeroInstance.h"
#include "Skill/KMSkillHandler.h"
#include "Tables/Generated/KMTable_Item.h"
#include "Util/KMUtil.h"

AKMCharacter::AKMCharacter(const FObjectInitializer& objectInitializer) :
	Super(objectInitializer.SetDefaultSubobjectClass<UKMSkeletalMeshComponent>(ACharacter::MeshComponentName).
		SetDefaultSubobjectClass<UKMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CurveWarping  = CreateDefaultSubobject<UKMCurveWarpingComponent>(TEXT("CurveWarping"));
	MartialArtsComponent = CreateDefaultSubobject<UKMMartialArtsComponent>(TEXT("MartialArts"));
	if (UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(GetCharacterMovement()))
	{
		characterMovement->CustomMovementDelegate.AddDynamic(CurveWarping, &UEMCurveWarpingComponent::OnCustomMovement);
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
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Landed"));
			skillHandler->TriggerEvent(FKMGameplayTagName::Event_Move_Landing_Tag);
		}
	}
}

UEMMartialArtsComponent* AKMCharacter::GetMartialArtsComponent() const
{
	return MartialArtsComponent;
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

UKMCharacterInstance* AKMCharacter::GetCharacterInstance() const
{
	return Cast<UKMCharacterInstance>(GetCharacterInstanceInternal());
}

void AKMCharacter::PossessedByCharacterInstance(UEMGameObjectInstance* newCharacterInstance)
{
	Super::PossessedByCharacterInstance(newCharacterInstance);

	if (UKMCharacterInstance* characterInstance = Cast<UKMCharacterInstance>(newCharacterInstance))
	{
		characterInstance->SetCharacter(this);
		characterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(GetActorForwardVector())), true);
	}
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