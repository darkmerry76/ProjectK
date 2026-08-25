#include "KMGameObjectSubsystem.h"
#include <Tables/Generated/KMTable_Interactive.h>
#include <Tables/Generated/KMTable_Skill_Normal.h>
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMCharacterPDA.h"
#include "DataAsset/KMInteractivePDA.h"
#include "GameActor/Camera/KMCameraActor.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameActor/Pawn/Interactive/KMInteractiveActorBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameObject/KMActorInstance.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/KMGhostInstance.h"
#include "GameObject/KMMonsterInstance.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tables/Generated/KMTable_Character.h"
#include "Tables/Generated/KMTable_Skill.h"
#include "Util/KMUtil.h"

UKMGameObjectSubsystem* UKMGameObjectSubsystem::GetGameObjectSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMGameObjectSubsystem>(GetSubsystem(worldContextObject, UKMGameObjectSubsystem::StaticClass()));
}

void UKMGameObjectSubsystem::Initialize()
{
	Super::Initialize();

	FWorldDelegates::OnPreWorldFinishDestroy.AddUObject(this, &ThisClass::OnPreWorldFinishDestroy);
}

void UKMGameObjectSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(this);
}

void UKMGameObjectSubsystem::InitializeActorFeatures()
{
	if (AKMCameraActor* cameraActor = Cast<AKMCameraActor>(UGameplayStatics::GetActorOfClass(this, AKMCameraActor::StaticClass())))
	{
		cameraActor->OnPostViewportOnAspectRatio();	
	}
}

int32 UKMGameObjectSubsystem::AddGameObject(UEMGameObjectInstance* newGameObjectInstance)
{
	int32 gameObjectIdx = Super::AddGameObject(newGameObjectInstance);
	GetSpawnGameObjectInstanceDelegate().Broadcast(Cast<UKMGameObjectInstance>(newGameObjectInstance));
	return gameObjectIdx;
}

void UKMGameObjectSubsystem::OnPreWorldFinishDestroy(UWorld* world)
{
	if(IsValid(world) == false)
	{
		return;
	}
	
	world->RemoveOnActorSpawnedHandler(ActorSpawnHandler);
}

UKMCharacterInstance* UKMGameObjectSubsystem::SpawnCharacterObject(FName characterTableId, const FTransform& transform, bool bFlipY)
{
	const FKMTable_CharacterRow* characterTable = FKMTable_CharacterRow::FindRowPtr(characterTableId);
	check (characterTable != nullptr);

	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(assetManager) == true);
	
	UKMCharacterPDA* characterPDA = Cast<UKMCharacterPDA>(assetManager->GetAsset(characterTable->pdaKey));
	check(IsValid(characterPDA) == true);
	
	UKMCharacterInstance* newCharacterInstance = NewObject<UKMCharacterInstance>(this, characterPDA->InstanceClass);
	newCharacterInstance->SetDepthSort(transform.GetLocation().X);
	newCharacterInstance->SetTable(characterTable);
	newCharacterInstance->SetTransform(transform);
	
	AKMCharacter* newCharacter = GetWorld()->SpawnActorDeferred<AKMCharacter>(
		characterPDA->CharacterClass, transform, nullptr, nullptr);
	check(IsValid(newCharacter) == true);

	newCharacter->SetMirror(bFlipY);
	newCharacter->PossessedByGameObjectInstance(newCharacterInstance);
	newCharacter->FinishSpawning(transform, false);
	newCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	
	AddGameObject(newCharacterInstance);

	return newCharacterInstance;
}

UKMInteractiveInstance* UKMGameObjectSubsystem::SpawnInteractiveObject(FName interactiveTableId, const FTransform& transform)
{
	const FKMTable_InteractiveRow* interactiveTableRow = FKMTable_InteractiveRow::FindRowPtr(interactiveTableId);
	check (interactiveTableRow);

	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(assetManager));
	
	UKMInteractivePDA* interactivePDA = Cast<UKMInteractivePDA>(assetManager->GetAsset(interactiveTableRow->AssetPda));
	check(IsValid(interactivePDA));
	
	UKMInteractiveInstance* newInteractiveInstance = NewObject<UKMInteractiveInstance>(this, interactivePDA->InstanceClass);
	newInteractiveInstance->SetTable(interactiveTableRow);

	AKMInteractiveActorBase* newInteractiveActor = GetWorld()->SpawnActorDeferred<AKMInteractiveActorBase>(
		interactivePDA->InteractiveClass, transform, nullptr, nullptr);
	check(IsValid(newInteractiveActor));

	newInteractiveActor->PossessedByGameObjectInstance(newInteractiveInstance);
	newInteractiveActor->FinishSpawning(transform, false);
	
	AddGameObject(newInteractiveInstance);

	return newInteractiveInstance;
}

UKMActorInstance* UKMGameObjectSubsystem::SpawnActorObject(TSubclassOf<AActor> actorClass, const FTransform& transform, int32 createdIndex, FKMOnActorInstancePreSpawn actorInstancePreSpawnDelegate)
{
	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(assetManager) == true);

	UKMActorInstance* newActorInstance = NewObject<UKMActorInstance>(this, UKMActorInstance::StaticClass());
	AActor* newActor = GetWorld()->SpawnActorDeferred<AActor>(
		actorClass, transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	check(IsValid(newActor) == true);

	newActorInstance->SetActor(newActor);
	actorInstancePreSpawnDelegate.ExecuteIfBound(newActorInstance, createdIndex);
	newActor->FinishSpawning(transform);
	
	newActorInstance->ActorInstanceDestroyedDelegate.BindUObject(this, &ThisClass::OnActorInstanceDestroyed);
	
	AddGameObject(newActorInstance);

	return newActorInstance;
}

void UKMGameObjectSubsystem::OnActorInstanceDestroyed(const UKMActorInstance* destroyedActorInstance)
{
	check(IsValid(destroyedActorInstance) == true);

	RemoveGameObject(destroyedActorInstance->GetId());
}

static bool CompareCharacterDistance(const UKMCharacterInstance* A, const UKMCharacterInstance* B, const FTransform& SourceTransform)
{
	return (A->GetTransform().GetTranslation() - SourceTransform.GetLocation()).SizeSquared() <
		   (B->GetTransform().GetTranslation() - SourceTransform.GetLocation()).SizeSquared();
}

int32 UKMGameObjectSubsystem::SkillForSearchForClosestTarget(
	const UKMCharacterInstance* attacker, const FKMSkillKey* skillKey, TArray<FKMObjectKey>& outCharacters) const
{
	check(IsValid(attacker) == true);

	const FTransform& sourceTransform = attacker->GetTransform();
	
	int32 bestTargetIndex = INDEX_NONE;
	int32 scopeCount = 0;

	TArray<TWeakObjectPtr<UKMCharacterInstance>> newOutCharacters;
	
	for (auto objectItr : GameObjectMap)
	{
		UKMCharacterInstance* target = Cast<UKMCharacterInstance>(objectItr.Value);
		if (IsValid(target) == false)
		{
			continue;
		}

		if (target->CanBeTargeted() == false)
		{
			continue;
		}

		if (skillKey != nullptr && UKMUtil::IsInTargetType(skillKey->TableRecord->TargetType, attacker, target) == false)
		{
			continue;
		}
		
		newOutCharacters.Emplace(target);
		scopeCount++;
	}
	newOutCharacters.Sort([&sourceTransform](const TWeakObjectPtr<UKMCharacterInstance> A, const TWeakObjectPtr<UKMCharacterInstance> B)
	{
		return CompareCharacterDistance(A.Get(), B.Get(), sourceTransform);
	});

	if (newOutCharacters.Num() != 0)
	{
		bestTargetIndex = 0;
	}
	
	if (skillKey != nullptr)
	{
		if (const FKMTable_Skill_NormalRow* normalSkillTable = CastRow<FKMTable_Skill_NormalRow>(skillKey->TableRecord))
		{
			if (normalSkillTable->ScopeType == EKMSkillScopeType::Single)
			{
				if (newOutCharacters.Num() > 1)
				{
					newOutCharacters.RemoveAt(1, newOutCharacters.Num() - 1);
				}
			}
			else if (normalSkillTable->MaxScopeCount < scopeCount && normalSkillTable->MaxScopeCount > 0)
			{
				newOutCharacters.RemoveAt(normalSkillTable->MaxScopeCount, newOutCharacters.Num() - 1);
			}
		}
	}

	for (auto objectItr : newOutCharacters)
	{
		outCharacters.Emplace(objectItr->GetId());
	}

	return bestTargetIndex;
}

void UKMGameObjectSubsystem::Tick(float deltaTime)
{
	float worldDeltaSeconds = GetWorld()->GetDeltaSeconds();
	for (auto objectItr = GameObjectMap.CreateIterator(); objectItr; ++objectItr)
	{
		float timeDilation = 1.f;
		if (UKMGameObjectInstance* gameObjectInstance = Cast<UKMGameObjectInstance>(objectItr.Value()))
		{
			timeDilation = gameObjectInstance->GetTimeDilation();

			if (AActor* ownerActor = gameObjectInstance->GetOwnerActor())
			{
				ownerActor->CustomTimeDilation = timeDilation;
			}
		}
		objectItr.Value()->Tick(worldDeltaSeconds * timeDilation);	
	}
}

UKMCharacterInstance* UKMGameObjectSubsystem::GetAuthCharacterInstance() const
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if(!IsValid(playerController))
	{
		return nullptr;
	}
	
	AKMCharacter* character = Cast<AKMCharacter>(playerController->GetPawn());
	if (!IsValid(character))
	{
		return nullptr;
	}
	
	return character->GetCharacterInstance();
}

template<typename _TL> int32 UKMGameObjectSubsystem::NumGameObject(bool bExcludeDead) const
{
	int numCount = 0;
	for (auto objectItr : GameObjectMap)
	{
		if (objectItr.Value->IsA<_TL>() == false)
		{
			continue;
		}
		
		if (UKMCharacterInstance* characterInstance = Cast<UKMCharacterInstance>(objectItr.Value))
		{
			if (characterInstance->IsDead() == true)
			{
				continue;
			}
		}
		numCount++;
	}
	return numCount;
}

int32 UKMGameObjectSubsystem::NumHero(bool bExcludeDead) const
{
	return NumGameObject<UKMCharacterInstance>(bExcludeDead);
}

int32 UKMGameObjectSubsystem::NumMonster(bool bExcludeDead) const
{
	return NumGameObject<UKMMonsterInstance>(bExcludeDead);
}

int32 UKMGameObjectSubsystem::NumGhost(bool bExcludeDead) const
{
	return NumGameObject<UKMGhostInstance>(bExcludeDead);
}

TStatId UKMGameObjectSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKMGameObjectSubsystem, STATGROUP_Tickables)
}

ETickableTickType UKMGameObjectSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool UKMGameObjectSubsystem::IsTickable() const
{
	return HasAnyFlags(RF_ClassDefaultObject) == false && GetWorld()->IsPaused() == false;
}
