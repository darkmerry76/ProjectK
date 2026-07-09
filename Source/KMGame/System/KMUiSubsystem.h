#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "System/EMGameObjectSubsystem.h"
#include "Ui/Window/Dialog/KMPopupMenuWidget.h"
#include "KMUiSubsystem.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EKMMenuType : uint8
{
	QuitPopup = 0,
	GameMenu = 1,
};

USTRUCT()
struct KMGAME_API FKMMenuEntry
{
	GENERATED_BODY()

	UPROPERTY()
	EKMMenuType Type = EKMMenuType::QuitPopup;
	
	UPROPERTY()
	TObjectPtr<UKMUserWidget> UserWidget;
};

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

	UPROPERTY()
	TArray<FKMMenuEntry> ShowWidgetStack;
	
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
	class UKMPrologueWindowWidget* CreatePrologue(FName prologueTableId) const;

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

	UFUNCTION(BlueprintPure)
	bool IsOpenedMenu(EKMMenuType menuType) const;

	UFUNCTION(BlueprintPure)
	bool HasMenu() const;

	UFUNCTION(BlueprintCallable)
	void OpenedMenu(EKMMenuType menuType, class UKMUserWidget* openedWidget);

	UFUNCTION(BlueprintCallable)
	void ClosedMenu(EKMMenuType menuType);

	UFUNCTION(BlueprintCallable)
	void ClosedMenuByIndex(int32 index);

	UFUNCTION(BlueprintCallable)
	void PopClosedMenu();

protected:
	void ShowQuitPopup();

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
};