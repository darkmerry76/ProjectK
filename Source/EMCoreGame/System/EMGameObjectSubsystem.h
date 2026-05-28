#pragma once

#include "CoreMinimal.h"
#include "EMGameInstanceSubsystem.h"
#include "EMGameObjectSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEMRemoveGameObjectDelegate, class UEMGameObjectInstance* gameObjectInstance);
UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMGameObjectSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

public:
	virtual int32 AddGameObject(class UEMGameObjectInstance* newGameObjectInstance);
	virtual bool RemoveGameObject(int32 gameObjectId);

	virtual void RemoveAllGameObjects(TArray<TSubclassOf<UEMGameObjectInstance>>* ignoreInstanceClasses = nullptr);

	UFUNCTION(BlueprintPure)
	class UEMGameObjectInstance* GetGameObject(int32 gameObjectId);

	UFUNCTION(BlueprintPure)
	bool HasGameObject(int32 gameObjectId) const;

	TMap<int32, TObjectPtr<class UEMGameObjectInstance>>& GetGameObjectMap() { return GameObjectMap; }
	FEMRemoveGameObjectDelegate RemoveGameObjectDelegate;

protected:
	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<class UEMGameObjectInstance>> GameObjectMap;

	uint32 LastetId = 0;
};