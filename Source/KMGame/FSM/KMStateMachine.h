#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMStateNode.h"
#include "KMStateMachine.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMStateMachine : public UObject
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TMap<FGameplayTag, TSubclassOf<UKMStateNode>> ConstructState;
	
	UPROPERTY()
	TObjectPtr<class UKMStateNode> RootStateNode;

	UPROPERTY()
	TObjectPtr<class UKMStateNode> CurrentStateNode;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<class UKMStateNode>> StateNodes;

public:
	virtual void Initialize();
	virtual void Deinitialize();
	
	void Tick(float deltaTime);
	void Start();

	UFUNCTION(BlueprintNativeEvent)
	void OnInitialize();
	
	UFUNCTION(BlueprintCallable)
	class UKMStateNode* AddState(const FGameplayTag newStateTag, TSubclassOf<class UKMStateNode> newStateClass);

	UFUNCTION(BlueprintCallable)
	void AddTransition(const FGameplayTag fromTag, const FGameplayTag toTag, FKMTransitionRule transitionRule);

	UFUNCTION(BlueprintCallable)
	void RemoveState(const FGameplayTag newStateTag);

	UFUNCTION(BlueprintPure)
	class UKMStateNode* GetState(const FGameplayTag newStateTag) const;

	UFUNCTION(BlueprintCallable)
	void SetRootState(const FGameplayTag newStateTag);
	
	UFUNCTION(BlueprintPure)
	class UKMStateNode* GetRootState() const;

	UFUNCTION(BlueprintPure)
	class UKMStateNode* GetCurrentState() const;

	UFUNCTION(BlueprintPure)
	class UObject* GetOwner() const;

protected:
	void Transition(class UKMStateNode* transitionNode);
};

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMStateMachineGameObjectInstance : public UKMStateMachine
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	class UKMGameObjectInstance* GetGameObjectInstance() const;

	UFUNCTION(BlueprintPure)
	bool HasGameplayTag(const FGameplayTag gameplayTag) const;
};