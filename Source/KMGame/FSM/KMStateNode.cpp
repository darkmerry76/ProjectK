#include "KMStateNode.h"

UKMStateNode::UKMStateNode(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMStateNode::SetStateTag(const FGameplayTag newStateTag)
{
	StateTag = newStateTag;
}

void UKMStateNode::AddChild(UKMStateNode* newChildNode, FKMTransitionRule transitionRule)
{
	if (!IsValid(newChildNode))
	{
		return;
	}
	
	Childrens.AddUnique(newChildNode);
	TransitionRules.Emplace(newChildNode, transitionRule);
	newChildNode->Parent = this;
}

void UKMStateNode::RemoveChild(UKMStateNode* childNode)
{
	Childrens.Remove(childNode);
	TransitionRules.Remove(childNode);
}

void UKMStateNode::Destroy()
{
	if (IsValid(Parent))
	{
		Parent->RemoveChild(this);
	}
}

void UKMStateNode::OnEnter_Implementation()
{
	ElapsedTime = 0.f;
}

void UKMStateNode::OnLeave_Implementation()
{
}

void UKMStateNode::OnEvent_Implementation(const FGameplayTag& gameplayTag)
{
}

void UKMStateNode::Tick(float deltaTime)
{
	ElapsedTime += deltaTime;
}

float UKMStateNode::GetElapsedTime() const
{
	return ElapsedTime;
}

const FGameplayTag& UKMStateNode::GetStateTag() const
{
	return StateTag;
}

UKMStateNode* UKMStateNode::GetTransitionNode() const
{
	for (auto transitionItr = TransitionRules.CreateConstIterator(); transitionItr; ++transitionItr)
	{
		bool bTransition = false;
		transitionItr.Value().Execute(this, transitionItr.Key(), bTransition);
		if (bTransition)
		{
			return transitionItr.Key();
		}
	}
	return nullptr;
}