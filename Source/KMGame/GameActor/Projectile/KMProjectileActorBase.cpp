#include "KMProjectileActorBase.h"

#include "Core/KMDefine.h"

AKMProjectileActorBase::AKMProjectileActorBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKMProjectileActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKMProjectileActorBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AKMProjectileActorBase::TriggerEvent(FGameplayTag eventTag, const FKMProjectileEventData& EventData)
{
	TriggerDeletgate.Broadcast(eventTag, EventData);
}

FPrimaryAssetId AKMProjectileActorBase::GetPrimaryAssetId() const
{
	const UClass* classPtr = GetClass();
	if (classPtr == nullptr)
	{
		return Super::GetPrimaryAssetId();
	}
	FString assetName = classPtr->GetName();
	assetName.RemoveFromEnd(TEXT("_C"));
	return FPrimaryAssetId("KMProjectileActorBase", *assetName);
}