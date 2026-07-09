#include "KMUiSubsystem.h"
#include "Components/CanvasPanel.h"
#include "UI/Component//KMNarrativeWidget.h"
#include "Ui/Component/KMRootWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "GameMode/KMGameModeHeroSelect.h"
#include "GameMode/KMGameModeStage.h"
#include "Kismet/GameplayStatics.h"
#include "Ui/Component/KMCinematicWidget.h"
#include "Ui/Window/Dialog/KMPopupMenuWidget.h"
#include "Ui/Window/GameMenu/KMGameMenuWindowWidget.h"
#include "Ui/Window/Prologue/KMPrologueWindow.h"
#include "Util/KMUtil.h"

UKMUiSubsystem* UKMUiSubsystem::GetUiSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMUiSubsystem>(GetSubsystem(worldContextObject, UKMUiSubsystem::StaticClass()));
}

UKMUiSubsystem::UKMUiSubsystem() : Super()
{
	RootClass = UKMRootWidget::StaticClass();
}

void UKMUiSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMUiSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKMUiSubsystem::CreateRoot()
{
	RootWidget = CreateWidget<UKMRootWidget>(GetWorld(), RootClass);
	RootWidget->AddToViewport();
}

void UKMUiSubsystem::AttachStateWidget(UKMUserWidget* newStateWidget)
{
	if (!IsValid(newStateWidget))
	{
		return;
	}
	if (IsValid(RootWidget) && IsValid(RootWidget->RootPanel))
	{
		if (UCanvasPanelSlot* stateWidgetSlot = Cast<UCanvasPanelSlot>(RootWidget->RootPanel->AddChild(newStateWidget)))
		{
			stateWidgetSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			stateWidgetSlot->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
		}
	}
}

void UKMUiSubsystem::PrintNarrativeMessage(const FString message, const FLinearColor color, bool messageClear)
{
	RootWidget->NarrativeWidget->SetMessageText(FText::FromString(message), color, messageClear);
}

void UKMUiSubsystem::ClearNarrativeMessage()
{
	RootWidget->NarrativeWidget->ClearText();
}

UKMPrologueWindowWidget* UKMUiSubsystem::CreatePrologue() const
{
	return CreateWidget<UKMPrologueWindowWidget>(GetWorld(), PrologueWidgetClass);	
}

UKMCinematicWidget* UKMUiSubsystem::DrawCienmaticImage(TSubclassOf<UKMCinematicWidget> cienmaticWidgetClass)
{
	UKMCinematicWidget* cinematicWidget = CreateWidget<UKMCinematicWidget>(GetWorld(), cienmaticWidgetClass);
	check(IsValid(cinematicWidget));
	
	if (UCanvasPanelSlot* cinematicWidgetSlot = Cast<UCanvasPanelSlot>(RootWidget->RootPanel->AddChild(cinematicWidget)))
	{
		cinematicWidgetSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		cinematicWidgetSlot->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	}
	return cinematicWidget;
}

UKMPopupMenuWidget* UKMUiSubsystem::ShowPopup(const FString& titleText, const FString& messageText, EKMPopupType popupType, FKMPopupSelectDelegate selectDelegate)
{
	if (!IsValid(RootWidget))
	{
		return nullptr;;
	}
	
	UKMPopupMenuWidget* newPopupMenuWidget = CreateWidget<UKMPopupMenuWidget>(GetWorld(), PopupWidgetClass);
	if (!IsValid(newPopupMenuWidget))
	{
		return nullptr;
	}

	newPopupMenuWidget->SetTitleText(titleText);
	newPopupMenuWidget->SetMessageText(messageText);
	newPopupMenuWidget->SetPopupType(popupType);
	newPopupMenuWidget->PopupSelectDelegate = selectDelegate;

	if (IsValid(RootWidget->RootPanel))
	{
		if (UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(RootWidget->RootPanel->AddChild(newPopupMenuWidget)))
		{
			canvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.f, 1.f));
			canvasSlot->SetPosition(FVector2D(0.f, 0.f));
			canvasSlot->SetSize(FVector2D(0.f, 0.f));
			canvasSlot->SetZOrder(10000.f);
		}
	}
	
	return newPopupMenuWidget;
}

void UKMUiSubsystem::SelectedTitleMenu(FName menuId)
{
	if (menuId == TEXT("NewGame"))
	{
		UGameplayStatics::OpenLevel(this, TEXT("HeroSelect_p"));
	}
	else if (menuId == TEXT("QuitGame"))
	{
		ShowQuitPopup();
	}
}

void UKMUiSubsystem::SelectedGameMenu(FName menuId)
{
	if (menuId == TEXT("Resume"))
	{
		CloseGameMenu();
	}
	else if (menuId == TEXT("ReturnToTitle"))
	{
		UGameplayStatics::OpenLevel(this, TEXT("Title_p"));
	}
	else if (menuId == TEXT("QuitGame"))
	{
		ShowQuitPopup();
	}
}

void UKMUiSubsystem::CloseQuitPopup()
{
	if (PopupMenuWidget.IsValid())
	{
		PopupMenuWidget->RemoveFromParent();
		PopupMenuWidget = nullptr;
	}
}

void UKMUiSubsystem::ShowQuitPopup()
{
	if (PopupMenuWidget.IsValid())
	{
		CloseQuitPopup();
		return;
	}
	
	PopupMenuWidget = ShowPopup(TEXT("Quit Game"), TEXT("Are you sure you want to quit?"), EKMPopupType::YesOrNo,
		FKMPopupSelectDelegate::CreateLambda([this](const EKMPopupButtonType& buttonType)
		{
			if (buttonType == EKMPopupButtonType::Yes)
			{
				UKMUtil::Shutdown(this);
			}
			else if (buttonType == EKMPopupButtonType::Cancel || buttonType == EKMPopupButtonType::No)
			{
				CloseQuitPopup();
			}
		}));
}

void UKMUiSubsystem::HandleEscape()
{
	if (AKMGameModeHeroSelect* heroGameMode = Cast<AKMGameModeHeroSelect>(UGameplayStatics::GetGameMode(this)))
	{
		ShowQuitPopup();
	}
	else if (AKMGameModeStage* stageGameMode = Cast<AKMGameModeStage>(UGameplayStatics::GetGameMode(this)))
	{
		ShowGameMenu();
	}
	else
	{
		ShowQuitPopup();
	}
}

void UKMUiSubsystem::CloseGameMenu()
{
	if (GameMenuWidget.IsValid())
	{
		GameMenuWidget->RemoveFromParent();
		GameMenuWidget = nullptr;
	}
}

void UKMUiSubsystem::ShowGameMenu()
{
	if (GameMenuWidget.IsValid())
	{
		CloseGameMenu();
		return;
	}
	
	GameMenuWidget = CreateWidget<UKMGameMenuWindowWidget>(GetWorld(), GameMenuWidgetClass);
	if (!GameMenuWidget.IsValid())
	{
		return;
	}

	if (IsValid(RootWidget->RootPanel))
	{
		if (UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(RootWidget->RootPanel->AddChild(GameMenuWidget.Get())))
		{
			canvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.f, 1.f));
			canvasSlot->SetPosition(FVector2D(0.f, 0.f));
			canvasSlot->SetSize(FVector2D(0.f, 0.f));
			canvasSlot->SetZOrder(10000.f);
		}
	}
}