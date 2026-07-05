#include "KMTitleMenuWidget.h"
#include "KMTitleMenuItemWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/Component/EMTextBlock.h"
#include "UI/Component/EMVerticalBox.h"

UKMTitleMenuWidget::UKMTitleMenuWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMTitleMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	Refresh();
}

void UKMTitleMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UKMTitleMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Clear();
}

void UKMTitleMenuWidget::Refresh()
{
	Clear();
	
	if (IsValid(MenuVerticalBox))
	{
		for(int32 itemIndex = 0; itemIndex < MenuVerticalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMTitleMenuItemWidget* titleMenuItem = Cast<UKMTitleMenuItemWidget>(MenuVerticalBox->GetChildAt(itemIndex));
			if (!IsValid(titleMenuItem))
			{
				continue;
			}
			
			if (itemIndex == 0)
			{
				SelectedMenu(titleMenuItem);
				titleMenuItem->CurrentAlpha = 1.f;
			}
			
			if (titleMenuItem->ClickedDelegate.IsAlreadyBound(this, &UKMTitleMenuWidget::OnMenuClicked))
			{
				titleMenuItem->ClickedDelegate.RemoveAll(this);
			}
			if (titleMenuItem->HoverDelegate.IsAlreadyBound(this, &UKMTitleMenuWidget::OnMenuHovered))
			{
				titleMenuItem->HoverDelegate.RemoveAll(this);
			}
			if (titleMenuItem->UnhoverDelegate.IsAlreadyBound(this, &UKMTitleMenuWidget::OnMenuUnhovered))
			{
				titleMenuItem->UnhoverDelegate.RemoveAll(this);
			}
			titleMenuItem->ClickedDelegate.AddDynamic(this, &UKMTitleMenuWidget::OnMenuClicked);
			titleMenuItem->HoverDelegate.AddDynamic(this, &UKMTitleMenuWidget::OnMenuHovered);
			titleMenuItem->UnhoverDelegate.AddDynamic(this, &UKMTitleMenuWidget::OnMenuUnhovered);
		}
	}
}

void UKMTitleMenuWidget::Clear()
{
	if (IsValid(MenuVerticalBox))
	{
		for (int32 itemIndex = 0; itemIndex < MenuVerticalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMTitleMenuItemWidget* titleMenuItem = Cast<UKMTitleMenuItemWidget>(MenuVerticalBox->GetChildAt(itemIndex));
			if (!IsValid(titleMenuItem))
			{
				continue;
			}
			
			titleMenuItem->ClickedDelegate.RemoveAll(this);
			titleMenuItem->HoverDelegate.RemoveAll(this);
			titleMenuItem->UnhoverDelegate.RemoveAll(this);
		}
	}
}

void UKMTitleMenuWidget::OnMenuClicked_Implementation(UKMTitleMenuItemWidget* titleMenuItem)
{
}

void UKMTitleMenuWidget::SelectedMenu(UKMTitleMenuItemWidget* titleMenuItem)
{
	if (IsValid(PrevSelectMenuItem))
	{
		PrevSelectMenuItem->UnhoveredAnimation();
	}
	titleMenuItem->HoveredAnimation();
	PrevSelectMenuItem = titleMenuItem;
}

void UKMTitleMenuWidget::OnMenuHovered_Implementation(UKMTitleMenuItemWidget* titleMenuItem)
{
	SelectedMenu(titleMenuItem);
}

void UKMTitleMenuWidget::OnMenuUnhovered_Implementation(UKMTitleMenuItemWidget* titleMenuItem)
{
	if (IsValid(PrevSelectMenuItem))
	{
		//PrevSelectMenuItem->UnhoveredAnimation();
		//PrevSelectMenuItem = nullptr;
	}
}

void UKMTitleMenuWidget::NativeTick(const FGeometry& myGeometry, float deltaTime)
{
	Super::NativeTick(myGeometry, deltaTime);

	float fontScaleResult = 0.f;

	for(int32 itemIndex = 0; itemIndex < MenuVerticalBox->GetChildrenCount(); ++itemIndex)
	{
		UKMTitleMenuItemWidget* titleMenuItem = Cast<UKMTitleMenuItemWidget>(MenuVerticalBox->GetChildAt(itemIndex));
		if (!IsValid(titleMenuItem) || !IsValid(titleMenuItem->MenuTextBlock))
		{
			continue;
		}
		const FSlateFontInfo& font = titleMenuItem->MenuTextBlock->GetFont();
		fontScaleResult += font.Size - 30.f;
	}
}