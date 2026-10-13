#include "KMHeadTagWidget.h"
#include "Components/Image.h"
#include "UI/Component/EMTextBlock.h"

UKMHeadTagWidget::UKMHeadTagWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{

}

void UKMHeadTagWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TitleText->SetVisibility(ESlateVisibility::Hidden);
	LockOnImage->SetVisibility(ESlateVisibility::Hidden);
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
	LockOnImage->SetVisibility(newVisibility);
}

ESlateVisibility UKMHeadTagWidget::GetVisibileLockOnImage() const
{
	return LockOnImage->GetVisibility();
}