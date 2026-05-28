#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "System/EMGameObjectSubsystem.h"
#include "KMTargetSubsystem.generated.h"

class KMGAME_API FKMTargetCluster
{
public:
	FKMTargetCluster(FKMObjectKey targetKey) : TargetKey(targetKey) { }
	
	FKMObjectKey TargetKey;
	TArray<FKMObjectKey> Attackers;
};

class KMGAME_API FKMLockOnCluster : public FGCObject
{
public:
	FKMLockOnCluster(UObject* ownerObject) : OwnerObject(ownerObject) { }
	FKMLockOnCluster(const FKMLockOnCluster& source);
	virtual ~FKMLockOnCluster() override { }

	TArray<FKMObjectKey> Targets;
	
	class UKMCharacterInstance* GetBestTarget() const;
	FKMObjectKey GetBestTargetKey() const;

	FTransform GetTargetTransform() const;

	bool IsBestTargetAvailable() const;

	int32 NumTarget() const { return Targets.Num(); }
	class UKMGameObjectInstance* GetTargetByIndex(int32 index) const;

	virtual void AddReferencedObjects(FReferenceCollector& referenceCollector) override;
	virtual FString GetReferencerName() const override { return TEXT("FKMLockOnCluster"); }

private:
	TWeakObjectPtr<UObject> OwnerObject = nullptr;
};

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMTargetSubsystem : public UEMGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	
public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMTargetSubsystem* GetTargetSubsystem(const UObject* worldContextObject);
	
	bool CharacterLockOnTarget(const class UKMCharacterInstance* attacker, const struct FKMSkillKey* skillKey);
	bool CharacterLockOnRelease(const class UKMCharacterInstance* attacker);

	bool IsCharacterLockOn(FKMObjectKey attackerKey) const;
	TSharedPtr<FKMLockOnCluster> GetLockOnCluster(FKMObjectKey attackerKey) const;
	TSharedPtr<FKMTargetCluster> GetTargetCluster(FKMObjectKey targetKey) const;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;

protected:
	TMap<FKMObjectKey, TSharedPtr<FKMLockOnCluster>> LockOnClusters;
	TMap<FKMObjectKey, TSharedPtr<FKMTargetCluster>> TargetClusters;
};