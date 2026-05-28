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

UEMGameObjectInstance* AEMCharacterBase::GetCharacterInstanceInternal() const
{
	if (CharacterInstance.IsValid() == false)
	{
		return nullptr;
	}
	return CharacterInstance.Get();	
}

void AEMCharacterBase::PossessedByCharacterInstance(UEMGameObjectInstance* newCharacterInstance)
{
	CharacterInstance = newCharacterInstance;
}

void AEMCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}