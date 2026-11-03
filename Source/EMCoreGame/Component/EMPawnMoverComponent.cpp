#include "EMPawnMoverComponent.h"

UEMPawnMoverComponent::UEMPawnMoverComponent() : Super()
{
}

void UEMPawnMoverComponent::InitializeComponent()
{
	bool bInitialized = HasBeenInitialized();
	TGuardValue<bool> initializeComponentGuard(bInitialized, true);

	UActorComponent::InitializeComponent();

	const UWorld* myWorld = GetWorld();
	if (IsValid(myWorld))
	{
		FindDefaultUpdatedComponent();
		if (IsValid(BackendClass))
		{
			UActorComponent* newLiaisonComp = Cast<UActorComponent>(GetOwner()->FindComponentByInterface<IMoverBackendLiaisonInterface>());
			if (!IsValid(newLiaisonComp))
			{
				newLiaisonComp = NewObject<UActorComponent>(this, BackendClass, TEXT("BackendLiaisonComponent"));
			}

			BackendLiaisonComp.SetObject(newLiaisonComp);
			BackendLiaisonComp.SetInterface(CastChecked<IMoverBackendLiaisonInterface>(newLiaisonComp));
			if (BackendLiaisonComp)
			{
				newLiaisonComp->RegisterComponent();
				if (!newLiaisonComp->HasBeenInitialized())
				{
					newLiaisonComp->InitializeComponent();
				}
				newLiaisonComp->SetNetAddressable();
			}
		}
		else
		{
			UE_LOGF(LogMover, Error, "No backend class set on %ls. Mover actor will not function.", *GetNameSafe(GetOwner()));
		}
	}

	FMoverSyncState defaultMoverSyncState;
	GetDefaultInputAndState(OUT CachedLastProducedInputCmd, OUT defaultMoverSyncState, OUT CachedLastAuxState);
	MoverSyncStateDoubleBuffer.SetBufferedData(defaultMoverSyncState);
	CachedLastUsedInputCmd = CachedLastProducedInputCmd;
	LastMoverDefaultSyncState = MoverSyncStateDoubleBuffer.GetReadable().SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
}

FVector UEMPawnMoverComponent::ConsumeInputVector() const
{
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(ownerPawn))
	{
		return FVector::ZeroVector;
	}
	return ownerPawn->Internal_ConsumeMovementInputVector();
}

void UEMPawnMoverComponent::SetAnimRootMotionTranslationScale(const FVector& newScale)
{
	RootMotionTranslationScale = newScale;
}

const FVector& UEMPawnMoverComponent::GetAnimRootMotionTranslationScale() const
{
	return RootMotionTranslationScale;
}

void UEMPawnMoverComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
}