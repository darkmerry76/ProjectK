#include "KMTargetSubsystem.h"
#include "Core/KMDefine.h"
#include "KMBattleSubsystem.h"
#include "KMGameObjectSubsystem.h"
#include "GameObject/KMCharacterInstance.h"

FKMLockOnCluster::FKMLockOnCluster(const FKMLockOnCluster& source)
{
	OwnerObject = source.OwnerObject;
	Targets = source.Targets;
}

UKMGameObjectInstance* FKMLockOnCluster::GetBestTarget() const
{
	check(OwnerObject.IsValid());
	
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(OwnerObject.Get());
	check(IsValid(gameObjectSubsystem));

	return Cast<UKMGameObjectInstance>(gameObjectSubsystem->GetGameObject(GetBestTargetKey()));
}

FKMObjectKey FKMLockOnCluster::GetBestTargetKey() const
{
	if (Targets.IsEmpty())
	{
		return INDEX_NONE;
	}
	return Targets[0];
}

FTransform FKMLockOnCluster::GetTargetTransform() const
{
	UKMGameObjectInstance* characterInstance = GetBestTarget();
	if (!IsValid(characterInstance))
	{
		return FTransform::Identity;
	}
	
	return characterInstance->GetTransform(); 
}

bool FKMLockOnCluster::IsBestTargetAvailable() const
{
	UKMGameObjectInstance* gameObjectInstance = GetBestTarget();
	if (!IsValid(gameObjectInstance))
	{
		return false;
	}
	
	if (gameObjectInstance->IsDead())
	{
		return false;
	}

	return true;
}

UKMGameObjectInstance* FKMLockOnCluster::GetTargetByIndex(int32 index) const
{
	check(Targets.Num() > index);
	check(OwnerObject.IsValid());
	
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(OwnerObject.Get());
	check(IsValid(gameObjectSubsystem));

	return Cast<UKMGameObjectInstance>(gameObjectSubsystem->GetGameObject(Targets[index]));
}

void FKMLockOnCluster::AddReferencedObjects(FReferenceCollector& referenceCollector)
{
	//referenceCollector.AddReferencedObject(OwnerObject);
}

UKMTargetSubsystem* UKMTargetSubsystem::GetTargetSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMTargetSubsystem>(GetSubsystem(worldContextObject, UKMTargetSubsystem::StaticClass()));
}

void UKMTargetSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMTargetSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UKMTargetSubsystem::IsCharacterLockOn(FKMObjectKey attackerKey) const
{
	return LockOnClusters.Contains(attackerKey);
}

TSharedPtr<FKMLockOnCluster> UKMTargetSubsystem::GetLockOnCluster(FKMObjectKey attackerKey) const
{
	const TSharedPtr<FKMLockOnCluster>* lockOnCluster = LockOnClusters.Find(attackerKey);
	if (!lockOnCluster)
	{
		return nullptr;
	}
	return *lockOnCluster;
}

TSharedPtr<FKMTargetCluster> UKMTargetSubsystem::GetTargetCluster(FKMObjectKey targetKey) const
{
	const TSharedPtr<FKMTargetCluster>* targetCluster = TargetClusters.Find(targetKey);
	if (!targetCluster)
	{
		return nullptr;
	}
	return *targetCluster;
}

bool UKMTargetSubsystem::CharacterLockOnTarget(const UKMCharacterInstance* attacker, const FKMSkillKey* skillKey)
{
	check(IsValid(attacker));
	if (!attacker->CanLockOn())
	{
		return false;
	}
	
	TSharedPtr<FKMLockOnCluster>* lockOnCluster = LockOnClusters.Find(attacker->GetId());
	if (lockOnCluster && lockOnCluster->IsValid())
	{
		CharacterLockOnRelease(attacker);
	}
	
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);

	TArray<FKMObjectKey> closetTargets;
	int32 bestTargetIndex = gameObjectSubsystem->SkillForSearchForClosestTarget(attacker, skillKey, closetTargets);
	if (bestTargetIndex == INDEX_NONE)
	{
		return false;
	}
	
	TSharedPtr<FKMLockOnCluster> newLockOnCluster = MakeShared<FKMLockOnCluster>(this);
	newLockOnCluster->Targets = closetTargets;
	LockOnClusters.Emplace(attacker->GetId(), newLockOnCluster);

	TSharedPtr<FKMTargetCluster>* targetCluster = TargetClusters.Find(newLockOnCluster->GetBestTargetKey());
	if (!targetCluster)
	{
		TSharedPtr<FKMTargetCluster> newTargetCluster = MakeShared<FKMTargetCluster>(newLockOnCluster->GetBestTargetKey());
		newTargetCluster->Attackers.Emplace(attacker->GetId());
		TargetClusters.Emplace(newLockOnCluster->GetBestTargetKey(), newTargetCluster);	
	}
	else
	{
		if(!(*targetCluster)->Attackers.Contains(attacker->GetId()))
		{
			(*targetCluster)->Attackers.Emplace(attacker->GetId());
		}
	}
	
	return true;
}

bool UKMTargetSubsystem::CharacterLockOnRelease(const UKMCharacterInstance* attacker)
{
	TSharedPtr<FKMLockOnCluster>* lockOnCluster = LockOnClusters.Find(attacker->GetId());
	if (!lockOnCluster || !lockOnCluster->IsValid())
	{
		return false;
	}

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	bool bIsRemoveLockOnCluster = false;
	for (auto targetItr = (*lockOnCluster)->Targets.CreateIterator(); targetItr; ++targetItr)
	{
		const UKMCharacterInstance* target = Cast<UKMCharacterInstance>(gameObjectSubsystem->GetGameObject(*targetItr));
		if (IsValid(target))
		{
			if (!target->IsDead())
			{
				continue;
			}
			TSharedPtr<FKMTargetCluster>* targetCluster = TargetClusters.Find(*targetItr);
			if (!targetCluster || !targetCluster->IsValid())
			{
				continue;
			}
			(*targetCluster)->Attackers.Remove(attacker->GetId());
			if ((*targetCluster)->Attackers.IsEmpty())
			{
				TargetClusters.Remove(*targetItr);
			}
		}
		if ((*lockOnCluster)->GetBestTargetKey() == *targetItr)
		{
			bIsRemoveLockOnCluster = true;	
		}
		targetItr.RemoveCurrent();
	}
	
	if ((*lockOnCluster)->Targets.IsEmpty() || bIsRemoveLockOnCluster)
	{
		LockOnClusters.Remove(attacker->GetId());
	}
	return true;
}

void UKMTargetSubsystem::Tick(float DeltaTime)
{
	check(IsValid(GetWorld()));
	if (GetWorld()->IsPaused())
	{
		return;
	}

/*	UKMBattleSubsystem* battleSubsystem = UKMBattleSubsystem::GetBattleSubsystem(this);
	check(IsValid(battleSubsystem));

	if (battleSubsystem->GetBattleState() != EKMBattleState::Playing)
	{
		return;
	}*/

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	for (auto objectItr : gameObjectSubsystem->GetGameObjectMap())
	{
		UKMCharacterInstance* attacker = Cast<UKMCharacterInstance>(objectItr.Value);
		if (!IsValid(attacker))
		{
			continue;
		}

		if (!attacker->CanLockOn())
		{
			continue;
		}

		if (IsCharacterLockOn(attacker->GetId()))
		{
			CharacterLockOnRelease(attacker);
		}
	}
}

TStatId UKMTargetSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKMGameObjectSubsystem, STATGROUP_Tickables)
}

ETickableTickType UKMTargetSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool UKMTargetSubsystem::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject) && !GetWorld()->IsPaused();
}
