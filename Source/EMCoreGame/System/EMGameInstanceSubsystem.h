#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EMGameInstanceSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UEMGameInstanceSubsystem* GetSubsystem(
		const UObject* worldContextObject, TSubclassOf<UGameInstanceSubsystem> subsystemClass);

	virtual void Initialize();
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category="GameInstanceSubsystem", meta=(AllowPrivateAccess=true))
	void OnInitalize();

private:
	virtual void Initialize(FSubsystemCollectionBase& collection) override;
};