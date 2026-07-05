#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "System/EMGameObjectSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "KMBattleSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FKMBattleStateChangedDelegate, EKMBattleState prevState, EKMBattleState nextState);

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMBattleSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	
public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMBattleSubsystem* GetBattleSubsystem(const UObject* worldContextObject);

	UKMBattleSubsystem();
	
	UFUNCTION(BlueprintCallable)
	void StartBattleState();
	
	UFUNCTION(BlueprintCallable)
	void ClearBattleState();
	
	UFUNCTION(BlueprintCallable)
	void EndBattleState();
	
	UFUNCTION(BlueprintCallable)
	void ClearStage();

	UFUNCTION(BlueprintPure)
	bool IsBattleClear() const;

	UFUNCTION(BlueprintCallable)
	EKMBattleState GetBattleState() const { return BattleState; }
	
	FKMBattleStateChangedDelegate BattleStateChangedDelegate;

private:
	void ChangeBattleState(EKMBattleState currentState);
	void OnRemoveGameObject(class UEMGameObjectInstance* gameObjectInstance);

	UFUNCTION()
	void OnSubLevelShown();

	void RemoveCurrentLevel();

private:
	EKMBattleState BattleState = EKMBattleState::None;

	UPROPERTY(Transient)
	class ULevelStreaming* CurrentLevel = nullptr;

	bool bIsBattleClear = false;
};