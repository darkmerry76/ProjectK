#include "KMIndicatorSubsystem.h"

#include "GameActor/KMPlacementPreviewActor.h"

UKMIndicatorSubsystem* UKMIndicatorSubsystem::GetIndicatorSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMIndicatorSubsystem>(GetSubsystem(worldContextObject, UKMIndicatorSubsystem::StaticClass()));
}

void UKMIndicatorSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMIndicatorSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKMIndicatorSubsystem::OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS)
{
	Super::OnPreWorldInitialization(newWorld, iVS);
	MeshComponents.Reset();
	LastComponentId = 0;
}

int32 UKMIndicatorSubsystem::SpawnPlacementMeshComponent(const UMeshComponent* originMeshComponent, const FTransform& newTransform)
{
	if (!IsValid(GetWorld()))
	{
		return INDEX_NONE;
	}

	if (!ensure(IsValid(PlacementPreviewActorClass)))
	{
		return INDEX_NONE;
	}
	
	AKMPlacementPreviewActor* newPlacementPreviewActor = Cast<AKMPlacementPreviewActor>(GetWorld()->SpawnActor(PlacementPreviewActorClass));
	if (!IsValid(newPlacementPreviewActor))
	{
		return INDEX_NONE;
	}
			
	UMeshComponent* newAddMeshComponent = DuplicateObject<UMeshComponent>(originMeshComponent, newPlacementPreviewActor);
	if (!IsValid(newAddMeshComponent))
	{
		return INDEX_NONE;
	}
	
	newPlacementPreviewActor->FinishAddComponent(newAddMeshComponent, false, FTransform::Identity);
	newAddMeshComponent->SetupAttachment(newPlacementPreviewActor->GetRootScene());
	newPlacementPreviewActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	newPlacementPreviewActor->SetActorTransform(newTransform);

	for (int32 materialIndex = 0; materialIndex < newAddMeshComponent->GetNumMaterials(); ++materialIndex)
	{
		newAddMeshComponent->SetMaterial(materialIndex, PlacementMaterial);
		newAddMeshComponent->CreateDynamicMaterialInstance(materialIndex);
	}

	MeshComponents.Emplace(LastComponentId, newAddMeshComponent);

	return LastComponentId++;
}

UMeshComponent* UKMIndicatorSubsystem::GetMeshComponent(int32 actorId) const
{
	const TObjectPtr<class UMeshComponent>* existMeshComponent = MeshComponents.Find(actorId);
	if (!existMeshComponent || !IsValid(*existMeshComponent))
	{
		return nullptr;
	}
	return *existMeshComponent;
}

void UKMIndicatorSubsystem::RemoveMeshComponent(int32 actorId)
{
	TObjectPtr<class UMeshComponent>* existMeshComponent = MeshComponents.Find(actorId);
	if (!existMeshComponent || !IsValid(*existMeshComponent))
	{
		return;
	}
	(*existMeshComponent)->GetOwner()->Destroy();
}