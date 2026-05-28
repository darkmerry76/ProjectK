#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "KMCommandLet.generated.h"

UCLASS()
class KMEDITOR_API UKMCommandlet : public UCommandlet
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual int32 Main(const FString& Params) override;

protected:
	void RegisterCommands();
	
protected:
	UPROPERTY()
	TMap<FName, class UKMCommand*> CommandMap;
};