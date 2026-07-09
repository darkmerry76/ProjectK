#include "KMTitleMenuWindowWidget.h"
#include "System/KMUiSubsystem.h"
#include "Ui/Window/Common/KMVerticalMenuItemWidget.h"

UKMTitleMenuWindowWidget::UKMTitleMenuWindowWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMTitleMenuWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(TitleMenu))
	{
		TitleMenu->SelectedDelegate.AddDynamic(this, &ThisClass::OnSelectedMenu);
	}
}

void UKMTitleMenuWindowWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(TitleMenu))
	{
		TitleMenu->SelectedDelegate.RemoveAll(this);
	}
}

void UKMTitleMenuWindowWidget::OnSelectedMenu_Implementation(UKMVerticalMenuItemWidget* verticalMenuItem)
{
	if (UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this))
	{
		uiSubsystem->SelectedTitleMenu(verticalMenuItem->MenuId);
	}
}