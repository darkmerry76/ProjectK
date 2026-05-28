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

void UEMBTServiceBase::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AIOwner != nullptr && (ReceiveActivationImplementations & FBTNodeBPImplementationHelper::AISpecific))
	{
		OnActivationAI(AIOwner, AIOwner->GetPawn());
	}
	else if (ReceiveActivationImplementations & FBTNodeBPImplementationHelper::Generic)
	{
		OnActivation(ActorOwner);
	}

	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}

void UEMBTServiceBase::OnActivationAI(AAIController* OwnerController, APawn* ControlledPawn)
{

}

void UEMBTServiceBase::OnActivation(AActor* OwnerActor)
{
}

void UEMBTServiceBase::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.HasAnyFlags(RF_BeginDestroyed) && OwnerComp.GetOwner())
	{
		BlueprintNodeHelpers::AbortLatentActions(OwnerComp, *this);

		if (AIOwner != nullptr && (ReceiveDeactivationImplementations & FBTNodeBPImplementationHelper::AISpecific))
		{
			OnDeactivationAI(AIOwner, AIOwner->GetPawn());
		}
		else if (ReceiveDeactivationImplementations & FBTNodeBPImplementationHelper::Generic)
		{
			OnDeactivation(ActorOwner);
		}
	}
	
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UEMBTServiceBase::OnDeactivationAI(AAIController* OwnerController, APawn* ControlledPawn)
{
}

void UEMBTServiceBase::OnDeactivation(AActor* OwnerActor)
{
}

void UEMBTServiceBase::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	if (ReceiveSearchStartImplementations != 0)
	{
		if (AIOwner != nullptr && (ReceiveSearchStartImplementations & FBTNodeBPImplementationHelper::AISpecific))
		{
			OnSearchStartAI(AIOwner, AIOwner->GetPawn());
		}
		else if (ReceiveSearchStartImplementations & FBTNodeBPImplementationHelper::Generic)
		{
			OnSearchStart(ActorOwner);
		}
	}
	
	Super::OnSearchStart(SearchData);
}

void UEMBTServiceBase::OnSearchStartAI(AAIController* OwnerController, class APawn* ControlledPawn)
{
}

void UEMBTServiceBase::OnSearchStart(AActor* OwnerActor)
{
}