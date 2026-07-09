#include "KMVerticalMenuWidget.h"
#include "KMVerticalMenuItemWidget.h"
#include "System/KMUiSubsystem.h"
#include "UI/Component/EMTextBlock.h"
#include "UI/Component/EMVerticalBox.h"

UKMVerticalMenuWidget::UKMVerticalMenuWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMVerticalMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	Refresh();
}

void UKMVerticalMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UKMVerticalMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Clear();
}

void UKMVerticalMenuWidget::Refresh()
{
	Clear();
	
	if (IsValid(MenuVerticalBox))
	{
		for(int32 itemIndex = 0; itemIndex < MenuVerticalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMVerticalMenuItemWidget* verticalMenuItem = Cast<UKMVerticalMenuItemWidget>(MenuVerticalBox->GetChildAt(itemIndex));
			if (!IsValid(verticalMenuItem))
			{
				continue;
			}
			
			if (itemIndex == 0)
			{
				SelectedMenu(verticalMenuItem);
				verticalMenuItem->StartAlpha = 0.f;
				verticalMenuItem->NextAlpha = 1.f;
				verticalMenuItem->CurrentAnimTime = verticalMenuItem->CurrentHoveredAnimationTime;
				verticalMenuItem->SetFontSizeByAlpha(1.f, false);
			}
			
			if (verticalMenuItem->PressedDelegate.IsAlreadyBound(this, &ThisClass::OnMenuPressed))
			{
				verticalMenuItem->PressedDelegate.RemoveAll(this);
			}
			if (verticalMenuItem->HoverDelegate.IsAlreadyBound(this, &ThisClass::OnMenuHovered))
			{
				verticalMenuItem->HoverDelegate.RemoveAll(this);
			}
			if (verticalMenuItem->UnhoverDelegate.IsAlreadyBound(this, &ThisClass::OnMenuUnhovered))
			{
				verticalMenuItem->UnhoverDelegate.RemoveAll(this);
			}
			verticalMenuItem->PressedDelegate.AddDynamic(this, &ThisClass::OnMenuPressed);
			verticalMenuItem->HoverDelegate.AddDynamic(this, &ThisClass::OnMenuHovered);
			verticalMenuItem->UnhoverDelegate.AddDynamic(this, &ThisClass::OnMenuUnhovered);
		}
	}
}

void UKMVerticalMenuWidget::Clear()
{
	if (IsValid(MenuVerticalBox))
	{
		for (int32 itemIndex = 0; itemIndex < MenuVerticalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMVerticalMenuItemWidget* titleMenuItem = Cast<UKMVerticalMenuItemWidget>(MenuVerticalBox->GetChildAt(itemIndex));
			if (!IsValid(titleMenuItem))
			{
				continue;
			}
			
			titleMenuItem->PressedDelegate.RemoveAll(this);
			titleMenuItem->HoverDelegate.RemoveAll(this);
			titleMenuItem->UnhoverDelegate.RemoveAll(this);
		}
	}
}

void UKMVerticalMenuWidget::OnMenuPressed_Implementation(UKMVerticalMenuItemWidget* verticalMenuItem)
{
	SelectedDelegate.Broadcast(verticalMenuItem);
}

void UKMVerticalMenuWidget::SelectedMenu(UKMVerticalMenuItemWidget* verticalMenuItem)
{
	if (IsValid(PrevSelectMenuItem))
	{
		PrevSelectMenuItem->UnhoveredAnimation();
	}
	verticalMenuItem->HoveredAnimation();
	PrevSelectMenuItem = verticalMenuItem;
}

void UKMVerticalMenuWidget::OnMenuHovered_Implementation(UKMVerticalMenuItemWidget* verticalMenuItem)
{
	SelectedMenu(verticalMenuItem);
}

void UKMVerticalMenuWidget::OnMenuUnhovered_Implementation(UKMVerticalMenuItemWidget* verticalMenuItem)
{
	if (IsValid(PrevSelectMenuItem))
	{
		//PrevSelectMenuItem->UnhoveredAnimation();
		//PrevSelectMenuItem = nullptr;
	}
}

void UKMVerticalMenuWidget::NativeTick(const FGeometry& myGeometry, float deltaTime)
{
	Super::NativeTick(myGeometry, deltaTime);

	float fontScaleResult = 0.f;

	for(int32 itemIndex = 0; itemIndex < MenuVerticalBox->GetChildrenCount(); ++itemIndex)
	{
		UKMVerticalMenuItemWidget* verticalMenuItem = Cast<UKMVerticalMenuItemWidget>(MenuVerticalBox->GetChildAt(itemIndex));
		if (!IsValid(verticalMenuItem) || !IsValid(verticalMenuItem->MenuTextBlock))
		{
			continue;
		}
		const FSlateFontInfo& font = verticalMenuItem->MenuTextBlock->GetFont();
		fontScaleResult += font.Size - 30.f;
	}
}