#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EMIsolationSubsystem.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMIsolationSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType)
class EMCOREGAME_API UEMIsolationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "worldContextObject"))
	static UEMIsolationSubsystem* GetIsolationSubsystem(UObject* worldContextObject);

	inline static const FName IsolationTag = FName("Isolation");

	FDelegateHandle ActorSpawnedHandle;

public:
	void EnterIsolation(const TArray<AActor*>& visibleActors);
	void LeaveIsolation();

	bool IsActivated() const;

	void AddExtraComponent(AActor* ownerActor, class UPrimitiveComponent* newPrimitiveComponent);

protected:
	void OnActorSpawned(AActor* newActor);
	void ActorPrimitveRenderPassHidden(AActor* actor);
	void ComponentRenderPassHidden(class UPrimitiveComponent* primitiveComponent);
	void OnLevelAdded(ULevel* level, UWorld* world);
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& collection) override;
	virtual void Deinitialize() override;
	virtual bool DoesSupportWorldType(const EWorldType::Type worldType) const override;

protected:
	bool bIsActiveIsolation = false;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> VisibleActors;
};