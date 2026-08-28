#include "KMSpawnActor.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameActor/Pawn/Interactive/KMInteractiveActorBase.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"
#include "Kismet/GameplayStatics.h"
#include "System/KMGameObjectSubsystem.h"

AKMSpawnCharacter::AKMSpawnCharacter(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMSpawnCharacter::BeginPlay()
{
	Super::BeginPlay();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UKMCharacterInstance* characterInstance = gameObjectSubsystem->SpawnCharacterObject(CharacterId, GetActorTransform());
	check(IsValid(characterInstance));
}

AKMSpawnInteractive::AKMSpawnInteractive(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMSpawnInteractive::BeginPlay()
{
	Super::BeginPlay();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UKMInteractiveInstance* interactiveInstance = gameObjectSubsystem->SpawnInteractiveObject(InteractiveId, GetActorTransform());
	check(IsValid(interactiveInstance));

	if (InteractiveId.ToString().Contains(TEXT("Rice")))
	{
		interactiveInstance = interactiveInstance;
	}
	
	AKMInteractiveActorBase* interactiveActor = Cast<AKMInteractiveActorBase>(interactiveInstance->GetOwnerActor());
	check(IsValid(interactiveActor));

	float halfHeight = 0.f;

	if(UCapsuleComponent* capsuleComponent = Cast<UCapsuleComponent>(interactiveActor->GetMovementShapeComponent()))
	{
		halfHeight = capsuleComponent->GetScaledCapsuleHalfHeight();
	}
	else if (UBoxComponent* boxComponent = Cast<UBoxComponent>(interactiveActor->GetMovementShapeComponent()))
	{
		halfHeight = boxComponent->GetScaledBoxExtent().Z;
	}
	else if (USphereComponent* sphereComponent = Cast<USphereComponent>(interactiveActor->GetMovementShapeComponent()))
	{
		halfHeight = sphereComponent->GetScaledSphereRadius();
	}
	
	FVector startLocation = GetActorLocation() + FVector(0.f, 0.f, halfHeight);
	FVector targetLocation = startLocation + FVector(0.f, 0.f, -10000.f);

	FHitResult hitResult;

	FCollisionObjectQueryParams objectQueryParams;
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(interactiveActor);

	bool bHit = GetWorld()->LineTraceSingleByObjectType(hitResult, startLocation,targetLocation, objectQueryParams, queryParams);
	if (bHit == true)
	{
		interactiveActor->SetActorLocation(
			FVector(startLocation.X, startLocation.Y,hitResult.ImpactPoint.Z + halfHeight));
	}
}