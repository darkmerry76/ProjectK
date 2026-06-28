#include "EMIsolationSubsystem.h"
#include "EngineUtils.h"
#include "LandscapeProxy.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
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

void UEMIsolationSubsystem::ComponentRenderPassHidden(UPrimitiveComponent* primitiveComponent)
{
	if (!IsValid(primitiveComponent))
	{
		return;
	}
	if (primitiveComponent->bRenderInMainPass && !primitiveComponent->ComponentHasTag(IsolationTag))
	{
		primitiveComponent->SetRenderInMainPass(false);
		primitiveComponent->SetRenderInDepthPass(false);
		primitiveComponent->ComponentTags.AddUnique(IsolationTag);
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
		TArray<UMeshComponent*> meshes;
		actor->GetComponents(meshes);

		if (!meshes.IsEmpty())
		{
			bIsActorHidden = false;
			for (UMeshComponent* mesh : meshes)
			{
				ComponentRenderPassHidden(mesh);
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
					primitive->SetRenderInMainPass(true);
					primitive->SetRenderInDepthPass(true);
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
			UPrimitiveComponent* primtive = *primtiveItr;
			if (primtive->ComponentHasTag(IsolationTag))
			{
				primtive->SetRenderInMainPass(true);
				primtive->SetRenderInDepthPass(true);
				primtive->ComponentTags.Remove(IsolationTag);
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