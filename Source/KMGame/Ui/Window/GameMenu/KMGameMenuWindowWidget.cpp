#include "KMGameMenuWindowWidget.h"
#include "System/KMUiSubsystem.h"
#include "Ui/Window/Common/KMVerticalMenuItemWidget.h"
#include "Ui/Window/Common/KMVerticalMenuWidget.h"

UKMGameMenuWindowWidget::UKMGameMenuWindowWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMGameMenuWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(GameMenu))
	{
		GameMenu->SelectedDelegate.AddDynamic(this, &ThisClass::OnSelectedMenu);
	}
}

void UKMGameMenuWindowWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(GameMenu))
	{
		GameMenu->SelectedDelegate.RemoveAll(this);
	}
}

void UKMGameMenuWindowWidget::OnSelectedMenu_Implementation(UKMVerticalMenuItemWidget* verticalMenuItem)
{
	if (UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this))
	{
		uiSubsystem->SelectedGameMenu(verticalMenuItem->MenuId);
	}
}