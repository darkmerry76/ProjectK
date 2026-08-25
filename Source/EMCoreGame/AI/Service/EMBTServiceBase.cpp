#include "EMBTServiceBase.h"
#include "AIController.h"
#include "BlueprintNodeHelpers.h"
#include "BehaviorTree/BTFunctionLibrary.h"

UEMBTServiceBase::UEMBTServiceBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	UClass* StopAtClass = UEMBTServiceBase::StaticClass();
	ReceiveTickImplementations = FBTNodeBPImplementationHelper::CheckEventImplementationVersion(TEXT("ReceiveTick"), TEXT("ReceiveTickAI"), *this, *StopAtClass);
	ReceiveActivationImplementations = FBTNodeBPImplementationHelper::CheckEventImplementationVersion(TEXT("ReceiveActivation"), TEXT("ReceiveActivationAI"), *this, *StopAtClass);
	ReceiveDeactivationImplementations = FBTNodeBPImplementationHelper::CheckEventImplementationVersion(TEXT("ReceiveDeactivation"), TEXT("ReceiveDeactivationAI"), *this, *StopAtClass);
	ReceiveSearchStartImplementations = FBTNodeBPImplementationHelper::CheckEventImplementationVersion(TEXT("ReceiveSearchStart"), TEXT("ReceiveSearchStartAI"), *this, *StopAtClass);

	bNotifyBecomeRelevant = ReceiveActivationImplementations != 0;
	bNotifyCeaseRelevant = ReceiveDeactivationImplementations != 0;
	bNotifyOnSearch = ReceiveSearchStartImplementations != 0;
	bNotifyTick = ReceiveTickImplementations != 0;
	bShowPropertyDetails = true;
	bCreateNodeInstance = true;
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		BlueprintNodeHelpers::CollectPropertyData(this, StopAtClass, PropertyData);
	}
}

#if WITH_EDITOR
bool UEMBTServiceBase::UsesBlueprint() const
{
	return false;
}
#endif

void UEMBTServiceBase::OnBecomeRelevant(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	if (IsValid(AIOwner) && (ReceiveActivationImplementations & FBTNodeBPImplementationHelper::AISpecific))
	{
		OnActivationAI(AIOwner, AIOwner->GetPawn());
	}
	else if (ReceiveActivationImplementations & FBTNodeBPImplementationHelper::Generic)
	{
		OnActivation(ActorOwner);
	}

	Super::OnBecomeRelevant(ownerComp, nodeMemory);
}

void UEMBTServiceBase::OnActivationAI(AAIController* ownerController, APawn* controlledPawn)
{

}

void UEMBTServiceBase::OnActivation(AActor* ownerActor)
{
}

void UEMBTServiceBase::OnCeaseRelevant(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	if (!ownerComp.HasAnyFlags(RF_BeginDestroyed) && IsValid(ownerComp.GetOwner()))
	{
		BlueprintNodeHelpers::AbortLatentActions(ownerComp, *this);

		if (IsValid(AIOwner) && (ReceiveDeactivationImplementations & FBTNodeBPImplementationHelper::AISpecific))
		{
			OnDeactivationAI(AIOwner, AIOwner->GetPawn());
		}
		else if (ReceiveDeactivationImplementations & FBTNodeBPImplementationHelper::Generic)
		{
			OnDeactivation(ActorOwner);
		}
	}
	
	Super::OnCeaseRelevant(ownerComp, nodeMemory);
}

void UEMBTServiceBase::OnDeactivationAI(AAIController* ownerController, APawn* controlledPawn)
{
}

void UEMBTServiceBase::OnDeactivation(AActor* ownerActor)
{
}

void UEMBTServiceBase::OnSearchStart(FBehaviorTreeSearchData& searchData)
{
	if (ReceiveSearchStartImplementations != 0)
	{
		if (IsValid(AIOwner) && (ReceiveSearchStartImplementations & FBTNodeBPImplementationHelper::AISpecific))
		{
			OnSearchStartAI(AIOwner, AIOwner->GetPawn());
		}
		else if (ReceiveSearchStartImplementations & FBTNodeBPImplementationHelper::Generic)
		{
			OnSearchStart(ActorOwner);
		}
	}
	
	Super::OnSearchStart(searchData);
}

void UEMBTServiceBase::OnSearchStartAI(AAIController* ownerController, APawn* controlledPawn)
{
}

void UEMBTServiceBase::OnSearchStart(AActor* ownerActor)
{
}