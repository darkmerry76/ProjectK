#include "KMTitleMenuItemWidget.h"
#include "UI/Component/EMButton.h"
#include "UI/Component/EMImage.h"
#include "UI/Component/EMTextBlock.h"

UKMTitleMenuItemWidget::UKMTitleMenuItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMTitleMenuItemWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetText(Text);
}

void UKMTitleMenuItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(MenuButton))
	{
		MenuButton->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
		MenuButton->OnHovered.AddDynamic(this, &ThisClass::OnHovered);
		MenuButton->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhovered);

		DefaultTextWidgetTransform = MenuTextBlock->GetRenderTransform();
	}
}

void UKMTitleMenuItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMTitleMenuItemWidget::SetText(const FString& newText)
{
	if (IsValid(MenuTextBlock))
	{
		MenuTextBlock->SetText(FText::FromString(newText));
		SetFontSizeByAlpha(0.f);
	}
}

void UKMTitleMenuItemWidget::OnClicked()
{
	ClickedDelegate.Broadcast(this);
}

void UKMTitleMenuItemWidget::OnHovered()
{
	HoverDelegate.Broadcast(this);
}

void UKMTitleMenuItemWidget::OnUnhovered()
{
	UnhoverDelegate.Broadcast(this);
}

void UKMTitleMenuItemWidget::SetFontSizeByAlpha(float alpha)
{
	if (IsValid(MenuTextBlock))
	{
		FSlateFontInfo textFont = MenuTextBlock->GetFont();
		textFont.Size = FMath::Lerp(NormalFontSize, HoveredFontSize, alpha);
		MenuTextBlock->SetFont(textFont);

		float DpiScale = 1.f;//UWidgetLayoutLibrary::GetViewportScale(this);
		
		FWidgetTransform widgetTransform = DefaultTextWidgetTransform;
		widgetTransform.Translation.Y -= ((textFont.Size - NormalFontSize) * 0.65f) / DpiScale;
		
		MenuTextBlock->SetRenderTransform(widgetTransform);
		FLinearColor fontColor = FMath::Lerp(NormalTextColor, SelectTextColor, alpha);
		MenuTextBlock->SetColorAndOpacity(fontColor);
	}
}

void UKMTitleMenuItemWidget::HoveredAnimation()
{
	NextAlpha = 1.f;
}

void UKMTitleMenuItemWidget::UnhoveredAnimation()
{
	NextAlpha = 0.f;
}

void UKMTitleMenuItemWidget::NativeTick(const FGeometry& geometry, float deltaTime)
{
	Super::NativeTick(geometry, deltaTime);

	CurrentAlpha = FMath::FInterpTo(CurrentAlpha, NextAlpha, deltaTime, HoveredAnimatinTime);
	SetFontSizeByAlpha(CurrentAlpha);
}