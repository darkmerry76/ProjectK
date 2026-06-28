#pragma once

#include "CoreMinimal.h"
#include "EMUtil.generated.h"

UCLASS(Blueprintable,BlueprintType)
class EMCOREGAME_API UEMUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="worldContextObject"))
	static bool IsGameWorld(UObject* worldContextObject);
};