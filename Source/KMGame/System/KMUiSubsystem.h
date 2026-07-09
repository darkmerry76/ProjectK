#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "System/EMGameObjectSubsystem.h"
#include "Ui/Window/Dialog/KMPopupMenuWidget.h"
#include "KMUiSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMUiSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMRootWidget> RootClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMPrologueWindowWidget> PrologueWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMPopupMenuWidget> PopupWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMGameMenuWindowWidget> GameMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UKMRootWidget> RootWidget;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class UKMPopupMenuWidget> PopupMenuWidget;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class UKMGameMenuWindowWidget> GameMenuWidget;
	
public:
	UKMUiSubsystem();
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMUiSubsystem* GetUiSubsystem(const UObject* worldContextObject);

	void CreateRoot();
	
	UFUNCTION(BlueprintCallable)
	void AttachStateWidget(class UKMUserWidget* newStateWidget);
	
	UFUNCTION(BlueprintCallable)
	void PrintNarrativeMessage(const FString message, const FLinearColor color = FLinearColor::White, bool messageClear = false);

	UFUNCTION(BlueprintCallable)
	void ClearNarrativeMessage();

	UFUNCTION(BlueprintCallable)
	class UKMPrologueWindowWidget* CreatePrologue() const;

	UFUNCTION(BlueprintCallable)
	class UKMCinematicWidget* DrawCienmaticImage(TSubclassOf<UKMCinematicWidget> cienmaticWidgetClass);

	UFUNCTION(BlueprintCallable)
	void SelectedTitleMenu(FName menuId);

	UFUNCTION(BlueprintCallable)
	void SelectedGameMenu(FName menuId);

	class UKMPopupMenuWidget* ShowPopup(const FString& titleText, const FString& messageText, EKMPopupType popupType, FKMPopupSelectDelegate selectDelegate);

	UFUNCTION(BlueprintCallable)
	void HandleEscape();

	UFUNCTION(BlueprintCallable)
	void ShowGameMenu();

protected:
	void ShowQuitPopup();
	void CloseQuitPopup();
	void CloseGameMenu();

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
};