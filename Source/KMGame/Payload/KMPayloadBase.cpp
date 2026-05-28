#include "KMPayloadBase.h"
#include "GameObject/KMCharacterInstance.h"

UKMPayloadBase::UKMPayloadBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMPayloadBase::Activate()
{
	
}

void UKMPayloadBase::Deactivate()
{
	
}

void UKMPayloadBase::OnActivate_Implementation()
{
	
}

void UKMPayloadBase::OnDeactivate_Implementation()
{
	
}

UKMCharacterInstance* UKMPayloadBase::GetCharacterInstance() const
{
	return GetTypedOuter<UKMCharacterInstance>();
}

AKMCharacter* UKMPayloadBase::GetCharacter() const
{
	UKMCharacterInstance* characterInstance = GetCharacterInstance();
	check(IsValid(characterInstance));
	return characterInstance->GetCharacter();
}

void UKMPayloadBase::OnHit(const FHitResult& HitResult)
{
	
}