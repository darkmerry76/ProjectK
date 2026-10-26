#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EMCharacterBase.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API AEMCharacterBase : public ACharacter
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void BeginPlay() override;
	static const FName MovementComponentName;
};