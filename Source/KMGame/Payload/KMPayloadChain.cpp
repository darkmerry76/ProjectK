#include "KMPayloadChain.h"
#include "Character/KMCharacter.h"

UKMPayloadChain::UKMPayloadChain(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMPayloadChain::Activate()
{
	Super::Activate();
}

void UKMPayloadChain::Deactivate()
{
	Super::Deactivate();
}

USkeletalMeshComponent* UKMPayloadChain::GetChainMesh() const
{
	AKMCharacter* character = Cast<AKMCharacter>(GetCharacter());
	check(IsValid(character));

	return character->GetMesh();
}