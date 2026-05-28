#pragma once

#include "CoreMinimal.h"
#include "KMNarrativeNode.generated.h"

DECLARE_DELEGATE_TwoParams(FKMNarrativeBranchDelegate, class UKMNarrativeNode* fromNode, FName toNodeId);
DECLARE_DELEGATE_OneParam(FKMNarrativeDestroyDelegate, class UKMNarrativeNode* fromNode);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMNarrativeNode : public UObject
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UKMNarrativeNode> ParentNode;
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<UKMNarrativeNode>> ChildNodes;

public:
	FKMNarrativeBranchDelegate BranchDelegate;
	FKMNarrativeDestroyDelegate DestroyDelegate;
	
	TArray<const struct FKMTable_NarrativeRow*> TableGroup;
	
public:
	virtual void Activate();
	virtual void Deactivate();
	virtual bool IsEnd() const;

	UKMNarrativeNode* GetParent() const;

	virtual void AddChild(UKMNarrativeNode* newChild);

	UFUNCTION(BlueprintImplementableEvent)
	void OnActivate();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeactivate();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTick(float deltaTime);
	
	virtual void CopyFrom(class UKMNarrativeNode* source);
	virtual void Tick(float deltaTime);

protected:
	virtual void OnChildNodeDestroy(UKMNarrativeNode* fromNode);
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};