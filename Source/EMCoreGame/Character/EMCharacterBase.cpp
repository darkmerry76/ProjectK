#include "EMCharacterBase.h"
#include "GameObject/EMGameObjectInstance.h"

const FName AEMCharacterBase::MovementComponentName = TEXT("MovementComponent");

AEMCharacterBase::AEMCharacterBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AEMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}