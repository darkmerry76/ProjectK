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

UKMPrologueWindowWidget* UKMUiSubsystem::CreatePrologue(FName prologueTableId) const
{
	UKMPrologueWindowWidget* newPrologueWidget = CreateWidget<UKMPrologueWindowWidget>(GetWorld(), PrologueWidgetClass);
	if (IsValid(RootWidget) && IsValid(newPrologueWidget))
	{
		newPrologueWidget->PrologueTableId = prologueTableId;
		if (IsValid(RootWidget->RootPanel))
		{
			if (UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(RootWidget->RootPanel->AddChild(newPrologueWidget)))
			{
				canvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.f, 1.f));
				canvasSlot->SetPosition(FVector2D(0.f, 0.f));
				canvasSlot->SetSize(FVector2D(0.f, 0.f));
				canvasSlot->SetZOrder(1000.f);
			}
		}
	}
	return newPrologueWidget;
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
	FTimerHandle timerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda([this, menuId]()
	{
		if (menuId == TEXT("NewGame"))
		{
			UGameplayStatics::OpenLevel(this, TEXT("HeroSelect_p"));
		}
		else if (menuId == TEXT("QuitGame"))
		{
			ShowQuitPopup();
		}
	}), 1.f, false);
}

void UKMUiSubsystem::SelectedGameMenu(FName menuId)
{
	if (menuId == TEXT("Resume"))
	{
		ClosedMenu(EKMMenuType::GameMenu);
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

void UKMUiSubsystem::ShowQuitPopup()
{
	if (IsOpenedMenu(EKMMenuType::QuitPopup))
	{
		ClosedMenu(EKMMenuType::QuitPopup);
		return;
	}
	
	UKMPopupMenuWidget* newPopupMenuWidget = ShowPopup(TEXT("Quit Game"), TEXT("Are you sure you want to quit?"), EKMPopupType::YesOrNo,
		FKMPopupSelectDelegate::CreateLambda([this](const EKMPopupButtonType& buttonType)
		{
			if (buttonType == EKMPopupButtonType::Yes)
			{
				UKMUtil::Shutdown(this);
			}
			else if (buttonType == EKMPopupButtonType::Cancel || buttonType == EKMPopupButtonType::No)
			{
				ClosedMenu(EKMMenuType::QuitPopup);
			}
		}));

	OpenedMenu(EKMMenuType::GameMenu, newPopupMenuWidget);
}

void UKMUiSubsystem::HandleEscape()
{
	if (HasMenu())
	{
		PopClosedMenu();
		return;
	}

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

void UKMUiSubsystem::ShowGameMenu()
{
	if (IsOpenedMenu(EKMMenuType::GameMenu))
	{
		ClosedMenu(EKMMenuType::GameMenu);
		return;
	}
	
	UKMGameMenuWindowWidget* newGameMenuWidget = CreateWidget<UKMGameMenuWindowWidget>(GetWorld(), GameMenuWidgetClass);
	if (!IsValid(newGameMenuWidget))
	{
		return;
	}

	if (IsValid(RootWidget->RootPanel))
	{
		if (UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(RootWidget->RootPanel->AddChild(newGameMenuWidget)))
		{
			canvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.f, 1.f));
			canvasSlot->SetPosition(FVector2D(0.f, 0.f));
			canvasSlot->SetSize(FVector2D(0.f, 0.f));
			canvasSlot->SetZOrder(10000.f);
		}
	}
	
	OpenedMenu(EKMMenuType::GameMenu, newGameMenuWidget);
}

void UKMUiSubsystem::OpenedMenu(EKMMenuType menuType, UKMUserWidget* openedWidget)
{
	if (!IsValid(openedWidget))
	{
		return;
	}
	FKMMenuEntry newMenuEntry;
	newMenuEntry.Type = menuType;
	newMenuEntry.UserWidget = openedWidget;
	ShowWidgetStack.Emplace(newMenuEntry);
}

void UKMUiSubsystem::ClosedMenuByIndex(int32 index)
{
	if (index != INDEX_NONE)
	{
		if (IsValid(ShowWidgetStack[index].UserWidget))
		{
			ShowWidgetStack[index].UserWidget->RemoveFromParent();
		}
		ShowWidgetStack.RemoveAt(index);
	}
}

void UKMUiSubsystem::ClosedMenu(EKMMenuType menuType)
{
	int32 menuIndex = ShowWidgetStack.IndexOfByPredicate([&menuType](const FKMMenuEntry& entry)
	{
		return entry.Type == menuType;
	});
	ClosedMenuByIndex(menuIndex);
}

void UKMUiSubsystem::PopClosedMenu()
{
	if (ShowWidgetStack.IsEmpty())
	{
		return;
	}
	
	int32 latestIndex = ShowWidgetStack.Num() - 1;
	ClosedMenuByIndex(latestIndex);
}

bool UKMUiSubsystem::IsOpenedMenu(EKMMenuType menuType) const
{
	return ShowWidgetStack.ContainsByPredicate([&menuType](const FKMMenuEntry& menuEntry)
	{
		return menuEntry.Type == menuType;
	});
}

bool UKMUiSubsystem::HasMenu() const
{
	return !ShowWidgetStack.IsEmpty();
}