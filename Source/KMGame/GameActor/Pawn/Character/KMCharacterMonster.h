#pragma once

#include "CoreMinimal.h"
#include "KMCharacter.h"
#include "KMCharacterMonster.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API AKMCharacterMonster : public AKMCharacter
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};