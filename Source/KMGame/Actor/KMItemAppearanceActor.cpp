#include "KMItemAppearanceActor.h"

#include "Character/KMChainAnimInstance.h"
#include "Character/KMCharacter.h"
#include "System/EMTickerSubsystem.h"

UKMItemAppearanceInstance::UKMItemAppearanceInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMItemAppearanceInstance::Spawn()
{
	check(IsValid(SpawnedActor) == false);
	
	if (AppearanceClass == nullptr)
	{
		return;
	}
	SpawnedActor = GetWorld()->SpawnActor<AKMItemAppearanceActor>(AppearanceClass);
}

void UKMItemAppearanceInstance::Despawn()
{
	if (IsValid(SpawnedActor) == true)
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
	if (SpawnedActor == nullptr)
	{
		return;
	}

	SpawnedActor->AttachToComponent(meshComponent,
		FAttachmentTransformRules::KeepRelativeTransform, socketName);

	SpawnedActor->GetRootComponent()->SetUsingAbsoluteRotation(true);
}

void UKMItemAppearanceInstance::Unequip(class USkeletalMeshComponent* meshComponent)
{
}

void UKMItemAppearanceInstance::Launch_Implementation()
{
	check(IsValid(SpawnedActor));
	
	USkeletalMeshComponent* skeletalMeshComponent = GetSpawnedkeletalMeshComponent();
	check(IsValid(skeletalMeshComponent));

	UKMChainAnimInstance* chainAnimInstance = Cast<UKMChainAnimInstance>(skeletalMeshComponent->GetAnimInstance());

	skeletalMeshComponent->SetRelativeRotation(GetCharacter()->GetActorRotation());

	skeletalMeshComponent->SetSimulatePhysics(false);

	chainAnimInstance->EnableAttack = true;
	chainAnimInstance->BlendAlpha = 0.f;

	skeletalMeshComponent->SnapshotPose(chainAnimInstance->Snapshot);
}

void UKMItemAppearanceInstance::Launching_Implementation(float alpha)
{
	check(IsValid(SpawnedActor));
	
	USkeletalMeshComponent* skeletalMeshComponent = GetSpawnedkeletalMeshComponent();
	check(IsValid(skeletalMeshComponent));

	UKMChainAnimInstance* chainAnimInstance = Cast<UKMChainAnimInstance>(skeletalMeshComponent->GetAnimInstance());
	chainAnimInstance->BlendAlpha = alpha;
}

void UKMItemAppearanceInstance::LaunchStop_Implementation()
{
	check(IsValid(SpawnedActor));
	
	USkeletalMeshComponent* skeletalMeshComponent = GetSpawnedkeletalMeshComponent();
	check(IsValid(skeletalMeshComponent));

	UKMChainAnimInstance* chainAnimInstance = Cast<UKMChainAnimInstance>(skeletalMeshComponent->GetAnimInstance());

	skeletalMeshComponent->SetSimulatePhysics(true);

	chainAnimInstance->EnableAttack = false;
	chainAnimInstance->BlendAlpha = 1.f;

	skeletalMeshComponent->ResetAllBodiesSimulatePhysics();
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

AKMItemAppearanceActor::AKMItemAppearanceActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

TObjectPtr<UKMItemAppearanceInstance> AKMItemAppearanceActor::CreateInstance(UObject* ownerObject) const
{
	UKMItemAppearanceInstance* newInstance = NewObject<UKMItemAppearanceInstance>(ownerObject);

	newInstance->AppearanceClass = GetClass();
	return newInstance;
}