#pragma once

#include "CoreMinimal.h"
#include "System/EMNetworkSubsystem.h"
#include "UObject/Object.h"
#include "KMNetworkSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMNetworkSubsystem : public UEMNetworkSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMNetworkSubsystem* GetNetworkSubsystem(const UObject* worldContextObject);

	UFUNCTION(BlueprintPure)
	const FGuid& GetAuthGuid() const;

	UPROPERTY()
	class UKMSaveGame* SaveGameInstance;

private:
	UPROPERTY(EditAnywhere, Transient)
	FGuid authAuthGuid = FGuid(0,0,0,0);
};
