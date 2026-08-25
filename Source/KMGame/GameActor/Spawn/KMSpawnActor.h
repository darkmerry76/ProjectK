#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KMSpawnActor.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract, hidecategories=Object)
class AKMSpawnCharacter : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterId = NAME_None;
	
protected:
	virtual void BeginPlay() override;
};

UCLASS(Blueprintable, BlueprintType, Abstract, hidecategories=Object)
class AKMSpawnInteractive : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InteractiveId = NAME_None;
	
protected:
	virtual void BeginPlay() override;
};