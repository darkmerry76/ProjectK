#include "EMIsolationSubsystem.h"
#include "EngineUtils.h"
#include "LandscapeProxy.h"
#include "NiagaraComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMIsolationSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UEMIsolationSubsystem* UEMIsolationSubsystem::GetIsolationSubsystem(UObject* worldContextObject)
{
	if (!IsValid(worldContextObject))
	{
		return nullptr;
	}
	UWorld* world = worldContextObject->GetWorld();
	if (!IsValid(world))
	{
		return nullptr;
	}
	return world->GetSubsystem<UEMIsolationSubsystem>();
}

void UEMIsolationSubsystem::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);
}

void UEMIsolationSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::LevelAddedToWorld.RemoveAll(this);
}

bool UEMIsolationSubsystem::IsActivated() const
{
	return bIsActiveIsolation;
}

void UEMIsolationSubsystem::AddExtraComponent(AActor* ownerActor, UPrimitiveComponent* newPrimitiveComponent)
{
	for (auto visibleActor : VisibleActors)
	{
		if (visibleActor == ownerActor)
		{
			ComponentRenderPassHidden(newPrimitiveComponent);
		}
	}
}

void UEMIsolationSubsystem::OnLevelAdded(ULevel* level, UWorld* world)
{
	if (world != GetWorld())
	{
		return;
	}
	
	if (!bIsActiveIsolation)
	{
		return;
	}
	for (AActor* actor : level->Actors)
	{
		ActorPrimitveRenderPassHidden(actor);
	}
}

void UEMIsolationSubsystem::ComponentRenderPassHidden(UPrimitiveComponent* primitiveComponent)
{
	if (!IsValid(primitiveComponent))
	{
		return;
	}
	if (USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(primitiveComponent))
	{
		if (skeletalMeshComponent->bRenderInMainPass && !skeletalMeshComponent->ComponentHasTag(IsolationTag))
		{
			skeletalMeshComponent->SetRenderInMainPass(false);
			skeletalMeshComponent->SetRenderInDepthPass(false);
			skeletalMeshComponent->ComponentTags.AddUnique(IsolationTag);
		}
	}
	else
	{
		if (primitiveComponent->IsVisible())
		{
			primitiveComponent->SetVisibility(false);
			primitiveComponent->ComponentTags.AddUnique(IsolationTag);
		}
	}
}

void UEMIsolationSubsystem::ActorPrimitveRenderPassHidden(AActor* actor)
{
	if (!IsValid(actor) || actor->IsA<ALight>() || actor->IsA<ASkyLight>())
	{
		return;
	}

	bool bIsActorHidden = true;

	if (!actor->IsA<ALandscapeProxy>())
	{
		TArray<UMeshComponent*> meshComponents;
		actor->GetComponents(meshComponents);

		if (!meshComponents.IsEmpty())
		{
			bIsActorHidden = false;
			for (UMeshComponent* primitiveComponent : meshComponents)
			{
				ComponentRenderPassHidden(primitiveComponent);
			}
		}
	}
	if (bIsActorHidden)
	{
		if (!actor->IsHidden())
		{
			actor->SetActorHiddenInGame(true);
			actor->Tags.AddUnique(IsolationTag);
		}
	}
}

void UEMIsolationSubsystem::OnActorSpawned(AActor* newActor)
{
	ActorPrimitveRenderPassHidden(newActor);
}

void UEMIsolationSubsystem::EnterIsolation(const TArray<AActor*>& visibleActors)
{
	if (!ensure(!bIsActiveIsolation))
	{
		return;
	}

	for (auto visibleActorItr : visibleActors)
	{
		VisibleActors.Emplace(visibleActorItr);
	}

	if (GetWorld()->IsGameWorld())
	{
		for (TActorIterator<AActor> actorItr(GetWorld()); actorItr; ++actorItr)
		{
			AActor* actor = *actorItr;
			if (visibleActors.Contains(actor))
			{
				continue;
			}
			ActorPrimitveRenderPassHidden(actor);
		}
	}
	else
	{
		for (TObjectIterator<UPrimitiveComponent> primtiveItr; primtiveItr; ++primtiveItr)
		{
			if (visibleActors.Contains(primtiveItr->GetOwner()))
			{
				continue;
			}
			if (primtiveItr->GetWorld() != GetWorld())
			{
				continue;
			}
			ComponentRenderPassHidden(*primtiveItr);
		}
	}
	
	bIsActiveIsolation = true;
	ActorSpawnedHandle = GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UEMIsolationSubsystem::OnActorSpawned));
}

void UEMIsolationSubsystem::LeaveIsolation()
{
	if (!ensure(bIsActiveIsolation))
	{
		return;
	}
	if (GetWorld()->IsGameWorld())
	{
		for (TActorIterator<AActor> actorItr(GetWorld()); actorItr; ++actorItr)
		{
			AActor* actor = *actorItr;
			
			TArray<UPrimitiveComponent*> primitives;
			actor->GetComponents(primitives);

			for (UPrimitiveComponent* primitive : primitives)
			{
				if (primitive->ComponentHasTag(IsolationTag))
				{
					if (USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(primitive))
					{
						skeletalMeshComponent->SetRenderInMainPass(true);
						skeletalMeshComponent->SetRenderInDepthPass(true);
					}
					else
					{
						primitive->SetVisibility(true);
					}
					primitive->ComponentTags.Remove(IsolationTag);
				}
			}

			if (actor->ActorHasTag(IsolationTag))
			{
				actor->SetActorHiddenInGame(false);
				actor->Tags.Remove(IsolationTag);
			}
		}
	}
	else
	{
		for (TObjectIterator<UPrimitiveComponent> primtiveItr; primtiveItr; ++primtiveItr)
		{
			if ((*primtiveItr)->ComponentHasTag(IsolationTag))
			{
				if (USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(*primtiveItr))
				{
					if (skeletalMeshComponent->ComponentHasTag(IsolationTag))
					{
						skeletalMeshComponent->SetRenderInMainPass(true);
						skeletalMeshComponent->SetRenderInDepthPass(true);
					}
				}
				else
				{
					(*primtiveItr)->SetVisibility(true);
				}
				(*primtiveItr)->ComponentTags.Remove(IsolationTag);
			}
		}
	}

	GetWorld()->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
	bIsActiveIsolation = false;
}

bool UEMIsolationSubsystem::DoesSupportWorldType(const EWorldType::Type worldType) const
{
	return true;
}