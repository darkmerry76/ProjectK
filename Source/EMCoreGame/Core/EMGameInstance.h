#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EMGameInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class EMCOREGAME_API UEMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEMGameInstance();
	
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintImplementableEvent, Category="GameInstance")
	void OnPostInitalize();

	UFUNCTION()
	void OnMinimumTimeEnd();
	
protected:
	void LoadBlueprintSubsystem();
	virtual void OnViewportCreated();
};
