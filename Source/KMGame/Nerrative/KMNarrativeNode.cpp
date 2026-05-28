#include "KMNarrativeNode.h"

UKMNarrativeNode::UKMNarrativeNode(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMNarrativeNode::Activate()
{
	
}

void UKMNarrativeNode::Deactivate()
{
	
}

bool UKMNarrativeNode::IsEnd() const
{
	return false;
}

void UKMNarrativeNode::CopyFrom(UKMNarrativeNode* source)
{
	check(IsValid(source));

	ChildNodes = source->ChildNodes; 
}

UKMNarrativeNode* UKMNarrativeNode::GetParent() const
{
	return ParentNode;
}

void UKMNarrativeNode::AddChild(UKMNarrativeNode* newChild)
{
	newChild->ParentNode = this;
	newChild->DestroyDelegate.BindUObject(this, &UKMNarrativeNode::OnChildNodeDestroy);

	ChildNodes.Emplace(newChild);
}

void UKMNarrativeNode::OnChildNodeDestroy(UKMNarrativeNode* fromNode)
{
	ChildNodes.Remove(fromNode);
}

FPrimaryAssetId UKMNarrativeNode::GetPrimaryAssetId() const
{
	const UClass* classPtr = GetClass();
	if (classPtr == nullptr)
	{
		return Super::GetPrimaryAssetId();
	}

	FString assetName = classPtr->GetName();
	assetName.RemoveFromEnd(TEXT("_C"));

	return FPrimaryAssetId("KMNarrativeNode", FName(*assetName));
}

void UKMNarrativeNode::Tick(float deltaTime)
{
	OnTick(deltaTime);

	for (auto childItr = ChildNodes.CreateIterator(); childItr; ++childItr)
	{
		if (!childItr)
		{
			continue;
		}
		(*childItr)->Tick(deltaTime);
	}

	if (IsEnd())
	{
		Deactivate();
		DestroyDelegate.Execute(this);
	}
}