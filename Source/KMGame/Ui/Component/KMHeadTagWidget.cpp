#include "KMHeadTagWidget.h"
#include "Components/Image.h"
#include "System/EMTweener.h"
#include "UI/Component/EMTextBlock.h"

UKMHeadTagWidget::UKMHeadTagWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHeadTagWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

UEMTextBlock* UKMHeadTagWidget::GetTitleText() const
{
	return TitleText;
}

UImage* UKMHeadTagWidget::GetLockOnImage() const
{
	return LockOnImage;
}

void UKMHeadTagWidget::SetVisibileTitleText(ESlateVisibility newVisibility)
{
	TitleText->SetVisibility(newVisibility);
}

ESlateVisibility UKMHeadTagWidget::GetVisibileTitleText() const
{
	return TitleText->GetVisibility();
}

void UKMHeadTagWidget::SetVisibileLockOnImage(ESlateVisibility newVisibility)
{
	if (newVisibility == ESlateVisibility::Hidden)
	{
		LockOnImage->SetOpacity(1.f);
	}
	else
	{
		LockOnImage->SetVisibility(newVisibility);
		LockOnImage->SetOpacity(0.f);
	}

	GaugeTweener = NewObject<UEMTweener>(this);
	GaugeTweener->AddTweenDelegate(FEMTweenDelegate::CreateLambda([this, newVisibility](eTickerEventType eventType, float prevPosition, float nextPosition, float eplipseTime, float duration)
	{
		float currEmpRatio = FMath::Min(eplipseTime / duration, 1.f);
		if (newVisibility == ESlateVisibility::Hidden)
		{
			LockOnImage->SetOpacity(1.f - currEmpRatio);
		}
		else
		{
			LockOnImage->SetOpacity(currEmpRatio);
		}

		if (eventType == eTickerEventType::REMOVED)
		{
			if (newVisibility == ESlateVisibility::Hidden)
			{
				LockOnImage->SetVisibility(newVisibility);
			}
		}
	}));
	GaugeTweener->Play(0.f, 1.f, 0.2f);
}

ESlateVisibility UKMHeadTagWidget::GetVisibileLockOnImage() const
{
	return LockOnImage->GetVisibility();
}