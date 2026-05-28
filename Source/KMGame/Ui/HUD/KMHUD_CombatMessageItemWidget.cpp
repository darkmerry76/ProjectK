#include "KMHUD_CombatMessageItemWidget.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Component/EMTextBlock.h"
#include "UI/Component/EMWidgetSwitcher.h"

UKMHUD_CombatMessageItemWidget::UKMHUD_CombatMessageItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHUD_CombatMessageItemWidget::SetMessage(const FText& messageText)
{
	MessageTextBlock->SetText(messageText);
}

void UKMHUD_CombatMessageItemWidget::SetMessageSwitch(int32 selectedIndex)
{
	MessageWidgetSwitcher->SetActiveWidgetIndex(selectedIndex);
}

void UKMHUD_CombatMessageItemWidget::SetDuration(float duration)
{
	Duration = duration;	
}

void UKMHUD_CombatMessageItemWidget::SetFontColor(const FLinearColor& color)
{
	FSlateColor fontColor = MessageTextBlock->GetColorAndOpacity();

	fontColor = color;
	
	MessageTextBlock->SetColorAndOpacity(fontColor);

}

void UKMHUD_CombatMessageItemWidget::SetFontSize(float newFontSize)
{
	FSlateFontInfo fontInfo = MessageTextBlock->GetFont();
	fontInfo.Size = newFontSize;
	
	MessageTextBlock->SetFont(fontInfo);
}

void UKMHUD_CombatMessageItemWidget::SetMessageType(EKMCommbatMessageType messageType)
{
	MessageType = messageType;
}

EKMCommbatMessageType UKMHUD_CombatMessageItemWidget::GetMessageType() const
{
	return MessageType;
}

UWidgetAnimation* UKMHUD_CombatMessageItemWidget::GetFadeInAnimation(int32 index) const
{
	return GetAnimationByName(*FString::Printf(TEXT("FadeIn_%d"), index));
}

UWidgetAnimation* UKMHUD_CombatMessageItemWidget::GetFadeOutAnimation(int32 index) const
{
	return GetAnimationByName(*FString::Printf(TEXT("FadeOut_%d"), index));
}

void UKMHUD_CombatMessageItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Refresh();
}

void UKMHUD_CombatMessageItemWidget::NativeDestruct()
{
	if (UWidgetAnimation* fadeInAnimation = GetFadeInAnimation(MessageWidgetSwitcher->GetActiveWidgetIndex()))
	{
		UnbindAllFromAnimationFinished(fadeInAnimation);
	}

	if (UWidgetAnimation* fadeOutAnimation = GetFadeOutAnimation(MessageWidgetSwitcher->GetActiveWidgetIndex()))
	{
		UnbindAllFromAnimationFinished(fadeOutAnimation);
	}

	if (GetWorld() != nullptr)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	
	Super::NativeDestruct();
}

void UKMHUD_CombatMessageItemWidget::Refresh()
{
	if (GetWorld() != nullptr)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	if (UWidgetAnimation* fadeInAnimation = GetFadeInAnimation(MessageWidgetSwitcher->GetActiveWidgetIndex()))
	{
		UnbindAllFromAnimationFinished(fadeInAnimation);
		
		FWidgetAnimationDynamicEvent newFadeInEvent;
		newFadeInEvent.BindDynamic(this, &ThisClass::OnFadeInComplate);
		BindToAnimationFinished(fadeInAnimation, newFadeInEvent);

		StopAnimation(fadeInAnimation);
		PlayAnimation(fadeInAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
	else
	{
		OnFadeInComplate();
	}

	if (UWidgetAnimation* fadeOutAnimation = GetFadeOutAnimation(MessageWidgetSwitcher->GetActiveWidgetIndex()))
	{
		UnbindAllFromAnimationFinished(fadeOutAnimation);
		FWidgetAnimationDynamicEvent newFadeOutEvent;
		newFadeOutEvent.BindDynamic(this, &ThisClass::OnFadeOutComplate);
		BindToAnimationFinished(fadeOutAnimation, newFadeOutEvent);
	}
}

void UKMHUD_CombatMessageItemWidget::OnFadeInComplate()
{
	check(GetWorld());
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnTimerComplate), Duration, false);
}

void UKMHUD_CombatMessageItemWidget::OnTimerComplate()
{
	if (UWidgetAnimation* fadeOutAnimation = GetFadeOutAnimation(MessageWidgetSwitcher->GetActiveWidgetIndex()))
	{
		StopAnimation(fadeOutAnimation);
		PlayAnimation(fadeOutAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
	else
	{
		OnFadeOutComplate();
	}

	TimerHandle.Invalidate();
}

void UKMHUD_CombatMessageItemWidget::OnFadeOutComplate()
{
	RemoveFromParent();
}