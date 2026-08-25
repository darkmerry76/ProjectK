#pragma once

#include "CoreMinimal.h"
#include "KMCharacter.h"
#include "KMCharacterBeast.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMCharacterBeast : public AKMCharacter
{
	GENERATED_BODY()
	
public:
	AKMCharacterBeast();

protected:
	virtual void BeginPlay() override;
};
