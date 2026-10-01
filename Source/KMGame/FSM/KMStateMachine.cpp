#include "KMStateMachine.h"
#include "KMStateNode.h"
#include "Core/KMGameplayTag.h"
#include "GameObject/KMGameObjectInstance.h"

UKMStateMachine::UKMStateMachine(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMStateMachine::Initialize()
{
	for (auto constructStateItr : ConstructState)
	{
		AddState(constructStateItr.Key, constructStateItr.Value);
	}
	
	OnInitialize();
}

void UKMStateMachine::OnInitialize_Implementation()
{
}

void UKMStateMachine::AddTransition(const FGameplayTag fromTag, const FGameplayTag toTag, FKMTransitionRule transitionRule)
{
	UKMStateNode* fromStateNode = GetState(fromTag);
	if (!ensure(IsValid(fromStateNode)))
	{
		return;
	}
	UKMStateNode* toStateNode = GetState(toTag);
	if (!ensure(IsValid(toStateNode)))
	{
		return;
	}

	fromStateNode->AddChild(toStateNode, transitionRule);
}

void UKMStateMachine::Deinitialize()
{
}

void UKMStateMachine::Start()
{
	Transition(RootStateNode);
}

void UKMStateMachine::Tick(float deltaTime)
{
	if (IsValid(CurrentStateNode))
	{
		CurrentStateNode->Tick(deltaTime);
		
		UKMStateNode* transitionNode = CurrentStateNode->GetTransitionNode();
		if (IsValid(transitionNode))
		{
			Transition(transitionNode);
		}
	}
}

void UKMStateMachine::Transition(UKMStateNode* transitionNode)
{
	if (!IsValid(transitionNode))
	{
		return;
	}
	
	if (IsValid(CurrentStateNode))
	{
		CurrentStateNode->OnLeave();
	}
	
	CurrentStateNode = transitionNode;
	CurrentStateNode->OnEnter();
}

UKMStateNode* UKMStateMachine::AddState(const FGameplayTag newStateTag, TSubclassOf<UKMStateNode> newStateClass)
{
	ensure(!StateNodes.Contains(newStateTag));
	
	UKMStateNode* newStateNode = NewObject<UKMStateNode>(this);
	newStateNode->SetStateTag(newStateTag);

	StateNodes.Emplace(newStateTag, newStateNode);

	return newStateNode;
}

void UKMStateMachine::RemoveState(const FGameplayTag newStateTag)
{
	ensure(StateNodes.Contains(newStateTag));

	TObjectPtr<UKMStateNode>* existStateNode = StateNodes.Find(newStateTag);
	if (!ensure(existStateNode))
	{
		return;
	}

	(*existStateNode)->Destroy();

	StateNodes.Remove(newStateTag);
}

UKMStateNode* UKMStateMachine::GetState(const FGameplayTag newStateTag) const
{
	const TObjectPtr<UKMStateNode>* existStateNode = StateNodes.Find(newStateTag);
	if (!ensure(existStateNode))
	{
		return nullptr;;
	}

	return *existStateNode;
}

void UKMStateMachine::SetRootState(const FGameplayTag newStateTag)
{
	UKMStateNode* stateNode = GetState(newStateTag);
	if (!ensure(IsValid(stateNode)))
	{
		return;
	}
	RootStateNode = stateNode;
}

UKMStateNode* UKMStateMachine::GetRootState() const
{
	return RootStateNode;
}

UKMStateNode* UKMStateMachine::GetCurrentState() const
{
	return CurrentStateNode;
}

UObject* UKMStateMachine::GetOwner() const
{
	return GetOuter();
}

UKMStateMachineGameObjectInstance::UKMStateMachineGameObjectInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

UKMGameObjectInstance* UKMStateMachineGameObjectInstance::GetGameObjectInstance() const
{
	return Cast<UKMGameObjectInstance>(GetOwner());
}

bool UKMStateMachineGameObjectInstance::HasGameplayTag(const FGameplayTag gameplayTag) const
{
	const UKMGameObjectInstance* gameObjectInstance = GetGameObjectInstance();
	if (!IsValid(gameObjectInstance))
	{
		return false;
	}
	return gameObjectInstance->HasGameplayTag(gameplayTag);
}