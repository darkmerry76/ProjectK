#include "KMCharacter.h"
#include "Actor/KMItemAppearanceActor.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMItemPDA.h"
#include "GameObject/KMGameObjectInstance.h"
#include "GameObject/KMHeroInstance.h"
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

UKMAnimationSetTag* AKMCharacter::GetAnimsetTag() const
{
	return AnimsetTag;
}

UKMItemAppearanceInstance* AKMCharacter::GetWeaponInstance() const
{
	return WeaponInstance;
}