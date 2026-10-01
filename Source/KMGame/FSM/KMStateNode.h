#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMStateNode.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FKMTransitionRule, const class UKMStateNode*, from, const class UKMStateNode*, to, bool&, bIsTransition);

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMStateNode : public UObject
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY()
	TArray<class UKMStateNode*> Childrens;

	UPROPERTY()
	TObjectPtr<class UKMStateNode> Parent;

	UPROPERTY()
	TMap<class UKMStateNode*, FKMTransitionRule> TransitionRules;

	FGameplayTag StateTag;

	float ElapsedTime = 0.f; 
	
public:
	void SetStateTag(const FGameplayTag newStateTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnter();

	UFUNCTION(BlueprintNativeEvent)
	void OnLeave();
	
	UFUNCTION(BlueprintNativeEvent)
    void OnEvent(const FGameplayTag& gameplayTag);
	
	UFUNCTION(BlueprintCallable)
	void AddChild(class UKMStateNode* newChildNode, FKMTransitionRule transitionRule);

	UFUNCTION(BlueprintCallable)
	void RemoveChild(class UKMStateNode* childNode);

	UFUNCTION(BlueprintCallable)
	void Destroy();

	void Tick(float deltaTime);

	UFUNCTION(BlueprintPure)
	float GetElapsedTime() const;

	UFUNCTION(BlueprintPure)
	const FGameplayTag& GetStateTag() const;

	UFUNCTION(BlueprintPure)
	class UKMStateNode* GetTransitionNode() const;
};