#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "System/EMGameObjectSubsystem.h"
#include "KMGameObjectSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMSpawnGameObjectInstanceDelegate, class UKMGameObjectInstance* gameObjectInstance);
DECLARE_DELEGATE_TwoParams(FKMOnActorInstancePreSpawn, class UKMActorInstance* actorInstance, int32 createdIndex);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMGameObjectSubsystem : public UEMGameObjectSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMGameObjectSubsystem* GetGameObjectSubsystem(const UObject* worldContextObject);

	UFUNCTION(BlueprintCallable)
	class UKMCharacterInstance* SpawnCharacterObject(FName characterTableId, const FTransform& transform, bool bFlipY = false);
	class UKMActorInstance* SpawnActorObject(TSubclassOf<class AActor> actorClass,
		const FTransform& transform, int32 createdIndex = 0, FKMOnActorInstancePreSpawn actorInstancePreSpawnDelegate = FKMOnActorInstancePreSpawn());

	int32 SkillForSearchForClosestTarget(
		const class UKMCharacterInstance* sourceCharacterInstance, const struct FKMSkillKey* skillKey, TArray<FKMObjectKey>& outCharacters) const;

	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetAuthCharacterInstance() const;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;

	UFUNCTION(BlueprintPure)
	int32 NumHero(bool bExcludeDead = true) const;

	UFUNCTION(BlueprintPure)
	int32 NumMonster(bool bExcludeDead = true) const;
	
	UFUNCTION(BlueprintPure)
	int32 NumGhost(bool bExcludeDead = true) const;

	void InitializeActorFeatures();

	void OnActorInstanceDestroyed(const class UKMActorInstance* destroyedActorInstance);
	
	static FKMSpawnGameObjectInstanceDelegate& GetSpawnGameObjectInstanceDelegate()
	{
		static FKMSpawnGameObjectInstanceDelegate SpawnGameObjectInstanceDelegate;
		return SpawnGameObjectInstanceDelegate;
	};
	
protected:
	template<typename _TL>
	int32 NumGameObject(bool bExcludeDead = true) const;
	
	void OnPreWorldFinishDestroy(UWorld* world);
	
	virtual void OnPostWorldInitialization(UWorld* loadedWorld, const UWorld::InitializationValues iVS) override;
	virtual void OnPostLoadMapWithWorld(UWorld* loadedWorld) override;

	virtual int32 AddGameObject(class UEMGameObjectInstance* newGameObjectInstance) override;

protected:
	FDelegateHandle ActorSpawnHandler;
};