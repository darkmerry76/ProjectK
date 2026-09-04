#pragma once

#include "CoreMinimal.h"
#include "System/EMGameInstanceSubsystem.h"
#include "KMIndicatorSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMIndicatorSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AKMPlacementPreviewActor> PlacementPreviewActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMaterialInterface> PlacementMaterial;
	
	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<class UMeshComponent>> MeshComponents;

	int32 LastComponentId = 0;
	
public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMIndicatorSubsystem* GetIndicatorSubsystem(const UObject* worldContextObject);

	UFUNCTION(BlueprintCallable)
	int32 SpawnPlacementMeshComponent(const UMeshComponent* originMeshComponent, const FTransform& newTransform);

	UFUNCTION(BlueprintPure)
	class UMeshComponent* GetMeshComponent(int32 meshComponentId) const;

	UFUNCTION(BlueprintCallable)
	void RemoveMeshComponent(int32 meshComponentId);
	
protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual void OnPreWorldInitialization(UWorld* newWorld, const UWorld::InitializationValues iVS) override;
};
