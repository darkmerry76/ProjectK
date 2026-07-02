#include "KMTitleMenuItemWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "System/EMTickerSubsystem.h"
#include "UI/Component/EMButton.h"
#include "UI/Component/EMTextBlock.h"

UKMTitleMenuItemWidget::UKMTitleMenuItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMTitleMenuItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(MenuButton))
	{
		MenuButton->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
		MenuButton->OnHovered.AddDynamic(this, &ThisClass::OnHovered);
		MenuButton->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhovered);

		DefaultTextWidgetTransform = MenuText->GetRenderTransform();
	}
}

void UKMTitleMenuItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMTitleMenuItemWidget::SetText(const FString& newText)
{
	if (IsValid(MenuText))
	{
		MenuText->SetText(FText::FromString(newText));
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
	if (IsValid(MenuText))
	{
		FSlateFontInfo textFont = MenuText->GetFont();
		textFont.Size = FMath::Lerp(NormalFontSize, HoveredFontSize, alpha);
		MenuText->SetFont(textFont);

		float DpiScale = 1.f;//UWidgetLayoutLibrary::GetViewportScale(this);
		
		FWidgetTransform widgetTransform = DefaultTextWidgetTransform;
		widgetTransform.Translation.Y -= ((textFont.Size - NormalFontSize) * 0.65f) / DpiScale;
		MenuText->SetRenderTransform(widgetTransform);
	}
}

void UKMTitleMenuItemWidget::HoveredAnimation()
{
	UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
	if (!IsValid(tickerSubsystem))
	{
		return;
	}
	float elipsedSeconds = 0.f;
	if (TickerHandle.IsValid())
	{
		elipsedSeconds = FMath::Max(0.f, HoveredAnimatinTime - TickerHandle.Data.Get()->GetElipsedSeconds(GetWorld()->GetTimeSeconds()));
		tickerSubsystem->RemoveTicker(TickerHandle);
	}
	TickerHandle = tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateUObject(this, &UKMTitleMenuItemWidget::OnHoveredAnimation), HoveredAnimatinTime, elipsedSeconds);
}

void UKMTitleMenuItemWidget::OnHoveredAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration)
{
	switch (eventType)
	{
	case eTickerEventType::CREATED:
	case eTickerEventType::UPDATED: SetFontSizeByAlpha(eplipseTime / duration); break;
	case eTickerEventType::REMOVED: SetFontSizeByAlpha(1.f); break;
	default: break;
	}
}

void UKMTitleMenuItemWidget::UnhoveredAnimation()
{
	UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
	if (!IsValid(tickerSubsystem))
	{
		return;
	}
	float elipsedSeconds = 0.f;
	if (TickerHandle.IsValid())
	{
		elipsedSeconds = FMath::Max(0.f, HoveredAnimatinTime - TickerHandle.Data.Get()->GetElipsedSeconds(GetWorld()->GetTimeSeconds()));
		tickerSubsystem->RemoveTicker(TickerHandle);
	}

	TickerHandle = tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateUObject(this, &UKMTitleMenuItemWidget::OnUnhoveredAnimation), HoveredAnimatinTime, elipsedSeconds);
}

void UKMTitleMenuItemWidget::OnUnhoveredAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration)
{
	switch (eventType)
	{
	case eTickerEventType::CREATED:
	case eTickerEventType::UPDATED: SetFontSizeByAlpha(1.f - (eplipseTime / duration)); break;
	case eTickerEventType::REMOVED: SetFontSizeByAlpha(0.f); break;
	default: break;
	}
}