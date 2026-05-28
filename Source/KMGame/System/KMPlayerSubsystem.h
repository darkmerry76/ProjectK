#pragma once

#include "CoreMinimal.h"
#include "System/EMPlayerSubsystem.h"
#include "KMPlayerSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMPlayerSubsystem : public UEMPlayerSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMPlayerSubsystem* GetPlayerSubsystem(const UObject* worldContextObject);
};