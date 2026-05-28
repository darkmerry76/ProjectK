#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "KMAuth.generated.h"

UCLASS(BlueprintType)
class KMGAME_API UKMAuth : public UObject
{
	GENERATED_BODY()

private:
	FGuid AccountGuid;

public:
	const FGuid& GetAccountGuid() const { return AccountGuid; }
};
