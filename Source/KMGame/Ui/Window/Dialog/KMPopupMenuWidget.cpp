#include "KMPopupMenuWidget.h"
#include "KMPopupMenuItemWidget.h"
#include "Components/Button.h"
#include "UI/Component/EMHorizontalBox.h"
#include "UI/Component/EMRichTextBlock.h"
#include "UI/Component/EMTextBlock.h"
#include "UI/Component/EMWidgetSwitcher.h"

void UKMPopupMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetPopupType(PopupType);
}

void UKMPopupMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (UEMHorizontalBox* horizontalBox = Cast<UEMHorizontalBox>(ButtonTypeSwitcher->GetActiveWidget()))
	{
		for (int32 itemIndex = 0; itemIndex < horizontalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMPopupMenuItemWidget* popupMenuItem = Cast<UKMPopupMenuItemWidget>(horizontalBox->GetChildAt(itemIndex));
			if (!IsValid(popupMenuItem))
			{
				continue;
			}
			popupMenuItem->GetButton()->OnPressed.RemoveAll(this);
		}
	}
}

void UKMPopupMenuWidget::SetTitleText(const FString newTitleText)
{
	if (IsValid(TitleTextBlock))
	{
		TitleTextBlock->SetText(FText::FromString(newTitleText));
	}
}

void UKMPopupMenuWidget::SetMessageText(const FString newMessageText)
{
	if (IsValid(MessageRichTextBlock))
	{
		MessageRichTextBlock->SetText(FText::FromString(newMessageText));
	}
}

void UKMPopupMenuWidget::SetPopupType(EKMPopupType newPopupType)
{
	PopupType = newPopupType;
	if (IsValid(ButtonTypeSwitcher))
	{
		ButtonTypeSwitcher->SetActiveWidgetIndex(static_cast<int32>(PopupType));
		
		if (UEMHorizontalBox* horizontalBox = Cast<UEMHorizontalBox>(ButtonTypeSwitcher->GetActiveWidget()))
		{
			for (int32 itemIndex = 0; itemIndex < horizontalBox->GetChildrenCount(); ++itemIndex)
			{
				UKMPopupMenuItemWidget* popupMenuItem = Cast<UKMPopupMenuItemWidget>(horizontalBox->GetChildAt(itemIndex));
				if (!IsValid(popupMenuItem))
				{
					continue;
				}

				popupMenuItem->GetButton()->OnPressed.RemoveAll(this);
				
				switch (popupMenuItem->GetButtonType())
				{
				case EKMPopupButtonType::Yes: popupMenuItem->GetButton()->OnPressed.AddDynamic(this, &ThisClass::OnYes); break;
				case EKMPopupButtonType::No: popupMenuItem->GetButton()->OnPressed.AddDynamic(this, &ThisClass::OnNo); break;
				case EKMPopupButtonType::Ok:  popupMenuItem->GetButton()->OnPressed.AddDynamic(this, &ThisClass::OnOk); break;
				case EKMPopupButtonType::Cancel:  popupMenuItem->GetButton()->OnPressed.AddDynamic(this, &ThisClass::OnCancel); break;
				default:break;
				}
			}
		}
	}
}

EKMPopupType UKMPopupMenuWidget::GetPopupType() const
{
	return PopupType;
}

void UKMPopupMenuWidget::OnSelectButton(const EKMPopupButtonType buttonType)
{
	PopupSelectDelegate.ExecuteIfBound(buttonType);
}

void UKMPopupMenuWidget::OnYes()
{
	OnSelectButton(EKMPopupButtonType::Yes);
}

void UKMPopupMenuWidget::OnNo()
{
	OnSelectButton(EKMPopupButtonType::No);
}

void UKMPopupMenuWidget::OnOk()
{
	OnSelectButton(EKMPopupButtonType::Ok);
}

void UKMPopupMenuWidget::OnCancel()
{
	OnSelectButton(EKMPopupButtonType::Cancel);
}