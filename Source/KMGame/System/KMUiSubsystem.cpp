#include "KMUiSubsystem.h"
#include "Components/CanvasPanel.h"
#include "UI/Component//KMNarrativeWidget.h"
#include "Ui/Component/KMRootWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Ui/Component/KMCinematicWidget.h"
#include "Ui/Window/Prologue/KMPrologueWindow.h"

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