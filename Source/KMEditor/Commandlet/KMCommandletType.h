#pragma once

#include "CoreMinimal.h"
#include "KMCommandLetType.generated.h"

UCLASS(abstract)
class KMEDITOR_API UKMCommand : public UObject
{
public:
	GENERATED_BODY()
	
	virtual int32 Main(const TArray<FString>& params) { return 1; };
	virtual FName GetCommandParamName() const;
};