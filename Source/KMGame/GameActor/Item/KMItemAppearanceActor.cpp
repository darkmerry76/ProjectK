#include "KMItemAppearanceActor.h"
#include "GameActor/Pawn/Character/KMChainAnimInstance.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"

UKMItemAppearanceInstance::UKMItemAppearanceInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMItemAppearanceInstance::Spawn()
{
	check(!IsValid(SpawnedActor));
	
	if (!AppearanceClass)
	{
		return;
	}
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = GetTypedOuter<AKMCharacter>();
	SpawnedActor = GetWorld()->SpawnActor<AKMItemAppearanceActor>(AppearanceClass, spawnParameters);
}

void UKMItemAppearanceInstance::Despawn()
{
	if (IsValid(SpawnedActor))
	{
		SpawnedActor->Destroy();
		SpawnedActor = nullptr;
	}
}

void UKMItemAppearanceInstance::BeginDestroy()
{
	Super::BeginDestroy();
	
	Despawn();
}

AKMCharacter* UKMItemAppearanceInstance::GetCharacter() const
{
	return GetTypedOuter<AKMCharacter>();
}

UKMCharacterInstance* UKMItemAppearanceInstance::GetCharacterInstance() const
{
	AKMCharacter* character = GetCharacter();
	check(IsValid(character));
	return character->GetCharacterInstance();
}

void UKMItemAppearanceInstance::Equip(class USkeletalMeshComponent* meshComponent, FName socketName)
{
	if (!SpawnedActor)
	{
		return;
	}

	SpawnedActor->AttachToComponent(meshComponent,
		FAttachmentTransformRules::KeepRelativeTransform, socketName);

	SpawnedActor->GetRootComponent()->SetUsingAbsoluteRotation(true);
	
	OnEquip(meshComponent, socketName);
}

void UKMItemAppearanceInstance::Unequip(class USkeletalMeshComponent* meshComponent)
{
	OnUnequip(meshComponent);
}

void UKMItemAppearanceInstance::Launch_Implementation()
{
	USkeletalMeshComponent* skeletalMeshComponent = GetSpawnedkeletalMeshComponent();
	check(IsValid(skeletalMeshComponent));

	skeletalMeshComponent->SetRelativeRotation(GetCharacter()->GetActorRotation());
	skeletalMeshComponent->SetSimulatePhysics(false);
	if (IsValid(SpawnedActor))
	{
		SpawnedActor->Launch();
	}
}

void UKMItemAppearanceInstance::Launching_Implementation(float alpha)
{
}

void UKMItemAppearanceInstance::LaunchStop_Implementation()
{
	USkeletalMeshComponent* skeletalMeshComponent = GetSpawnedkeletalMeshComponent();
	check(IsValid(skeletalMeshComponent));

	skeletalMeshComponent->SetSimulatePhysics(false);
	skeletalMeshComponent->ResetAllBodiesSimulatePhysics();
	if (IsValid(SpawnedActor))
	{
		SpawnedActor->LaunchStop();
	}
}

USkeletalMeshComponent* UKMItemAppearanceInstance::GetSpawnedkeletalMeshComponent() const
{
	if (!IsValid(SpawnedActor))
	{
		return nullptr;
	}
	
	USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(SpawnedActor->GetRootComponent());
	
	return skeletalMeshComponent;
}

AKMItemAppearanceActor* UKMItemAppearanceInstance::GetSpawnedActor() const
{
	return SpawnedActor;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AKMItemAppearanceActor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AKMItemAppearanceActor::AKMItemAppearanceActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

TObjectPtr<UKMItemAppearanceInstance> AKMItemAppearanceActor::CreateInstance(UObject* ownerObject, TSubclassOf<UKMItemAppearanceInstance> instanceClass) const
{
	UKMItemAppearanceInstance* newInstance = NewObject<UKMItemAppearanceInstance>(ownerObject, instanceClass);

	newInstance->AppearanceClass = GetClass();
	return newInstance;
}

void AKMItemAppearanceActor::SetVisbility(bool bIsVisibility)
{
#if WITH_EDITOR
	if (!GetWorld()->IsGameWorld())
	{
		SetIsTemporarilyHiddenInEditor(!bIsVisibility);
	}
	else
	{
		SetActorHiddenInGame(!bIsVisibility);	
	}
#else
	SetActorHiddenInGame(!bIsVisibility);
#endif
}

UKMCharacterInstance* AKMItemAppearanceActor::GetCharacterInstance() const
{
	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return nullptr;
	}
	return ownerCharacter->GetCharacterInstance();
}

AKMCharacter* AKMItemAppearanceActor::GetCharacter() const
{
	return Cast<AKMCharacter>(GetOwner());
}

void AKMItemAppearanceActor::Launch_Implementation()
{
}

void AKMItemAppearanceActor::LaunchStop_Implementation()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AKMItemAppearanceChainActor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AKMItemAppearanceChainActor::AKMItemAppearanceChainActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	ChainMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChainMesh"));
	ChainMesh->SetupAttachment(GetRootComponent());

	ChainMesh->SetCustomDepthStencilValue(1);
	ChainMesh->SetRenderCustomDepth(true);

	PrimaryActorTick.bCanEverTick = true;
}

TObjectPtr<UKMItemAppearanceInstance> AKMItemAppearanceChainActor::CreateInstance(UObject* ownerObject, TSubclassOf<UKMItemAppearanceInstance> instanceClass) const
{
	UKMItemAppearanceInstance* newInstance = NewObject<UKMItemAppearanceInstance>(ownerObject, instanceClass);
	newInstance->AppearanceClass = GetClass();
	return newInstance;
}

void AKMItemAppearanceChainActor::Launch_Implementation()
{
	UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
	check(IsValid(tickerSubsystem));

	TWeakObjectPtr<UKMChainAnimInstance> chainAnimInstance = Cast<UKMChainAnimInstance>(ChainMesh->GetAnimInstance());
	if (!chainAnimInstance.IsValid())
	{
		return;
	}

	if (UKMCharacterInstance* ownerCharacterInstance = GetCharacterInstance())
	{
		ownerCharacterInstance->HitCheckClear();
	}
	if (IsValid(ChainMesh))
	{
		FTransform socketTransform = ChainMesh->GetSocketTransform(RingSocketName);
		socketTransform.SetScale3D(FVector(RingRadius));
		PreviousTransform = socketTransform;
	}
	
	tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateLambda([this, chainAnimInstance](eTickerEventType eventType, float deltaSeconds, float elipsedTime, float duration)
	{
		switch (eventType)
		{
		case eTickerEventType::CREATED:
			chainAnimInstance->EnableAttack = true;
			chainAnimInstance->BlendAlpha = 0.f;
			SetVisbility(true);
			break;
		case eTickerEventType::UPDATED:
			chainAnimInstance->BlendAlpha = (elipsedTime / duration) * ChainLengthWeight;
			bIsCollisionCheck = true;
			break;
		case eTickerEventType::REMOVED:
			chainAnimInstance->BlendAlpha = ChainLengthWeight;
			bIsCollisionCheck = false;
			break;
		default:break;
		}
	}), ThrowDuration, 0.f);
}

void AKMItemAppearanceChainActor::LaunchStop_Implementation()
{
	UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
	check(IsValid(tickerSubsystem));

	UKMCharacterInstance* ownerCharacterInstance = GetCharacterInstance();
	if (!IsValid(ownerCharacterInstance))
	{
		return;
	}

	TWeakObjectPtr<UKMChainAnimInstance> chainAnimInstance = Cast<UKMChainAnimInstance>(ChainMesh->GetAnimInstance());
	if (!chainAnimInstance.IsValid())
	{
		return;
	}
	tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateLambda([this, chainAnimInstance, ownerCharacterInstance](eTickerEventType eventType, float deltaSeconds, float elipsedTime, float duration)
	{
		switch (eventType)
		{
		case eTickerEventType::CREATED:
			chainAnimInstance->EnableAttack = true;
			chainAnimInstance->BlendAlpha = ChainLengthWeight;
			break;
		case eTickerEventType::UPDATED:
			chainAnimInstance->BlendAlpha = (1.f - (elipsedTime / duration)) * ChainLengthWeight;
			break;
		case eTickerEventType::REMOVED:
			chainAnimInstance->BlendAlpha = 0.f;
			SetVisbility(false);
			ownerCharacterInstance->HitCheckClear();
			break;
		default:break;
		}
	}), GetOverDuration, 0.f);
}

void AKMItemAppearanceChainActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKMItemAppearanceChainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCollisionCheck && IsValid(ChainMesh))
	{
		if (UKMCharacterInstance* ownerCharacterInstance = GetCharacterInstance())
		{
			FTransform socketTransform = ChainMesh->GetSocketTransform(RingSocketName);
			socketTransform.SetScale3D(FVector(RingRadius));
			ownerCharacterInstance->BoxHitImpact(ownerCharacterInstance->GetSkillHandler()->GetLatestActiveSkillInstance(),
				PreviousTransform, socketTransform, { UEngineTypes::ConvertToObjectType(ECC_Damage) }, ACharacter::StaticClass(), NAME_None);

			PreviousTransform = socketTransform;
		}
	}
}