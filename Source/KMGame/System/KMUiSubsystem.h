#pragma once

#include "CoreMinimal.h"
#include "System/EMGameObjectSubsystem.h"
#include "KMUiSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMUiSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMRootWidget> RootClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMNarrativeWidget> NarrativeWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UKMRootWidget> RootWidget;
	
public:
	UKMUiSubsystem();
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMUiSubsystem* GetUiSubsystem(const UObject* worldContextObject);

	void Activate();
	
	UFUNCTION(BlueprintCallable)
	void PrintNarrativeMessage(const FString message, const FLinearColor color = FLinearColor::White, bool messageClear = false);

	UFUNCTION(BlueprintCallable)
	void ClearNarrativeMessage();

	UFUNCTION(BlueprintCallable)
	class UKMCinematicWidget* DrawCienmaticImage(TSubclassOf<UKMCinematicWidget> cienmaticWidgetClass);

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
};