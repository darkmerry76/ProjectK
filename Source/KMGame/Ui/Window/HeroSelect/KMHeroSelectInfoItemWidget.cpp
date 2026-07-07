#include "KMHeroSelectInfoItemWidget.h"
#include "UI/Component/EMHorizontalBox.h"
#include "UI/Component/EMTextBlock.h"

UKMHeroSelectInfoItemWidget::UKMHeroSelectInfoItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHeroSelectInfoItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Refresh();
}

void UKMHeroSelectInfoItemWidget::Refresh()
{
	if (IsValid(InfoHorizontalBox))
	{
	}
}

void UKMHeroSelectInfoItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMHeroSelectInfoItemWidget::SetTitleText(const FString& newTitleText)
{
	if (IsValid(TitleTextBlock))
	{
		TitleTextBlock->SetText(FText::FromString(newTitleText));
	}
}

void UKMHeroSelectInfoItemWidget::SetContentText(const FString& newContentText)
{
	if (IsValid(ContentTextBlock))
	{
		ContentTextBlock->SetText(FText::FromString(newContentText));
	}
}
