#include "KMPopupMenuItemWidget.h"

void UKMPopupMenuItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UKMPopupMenuItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

EKMPopupButtonType UKMPopupMenuItemWidget::GetButtonType() const
{
	return ButtonType;
}

UButton* UKMPopupMenuItemWidget::GetButton() const
{
	return Button;	
}

UTextBlock* UKMPopupMenuItemWidget::GetButtonTextBlock() const
{
	return ButtonTextBlock;
}