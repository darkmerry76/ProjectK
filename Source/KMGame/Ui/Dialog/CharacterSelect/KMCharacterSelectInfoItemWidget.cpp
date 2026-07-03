#include "KMCharacterSelectInfoItemWidget.h"
#include "UI/Component/EMHorizontalBox.h"
#include "UI/Component/EMTextBlock.h"

UKMCharacterSelectInfoItemWidget::UKMCharacterSelectInfoItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCharacterSelectInfoItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Refresh();
}

void UKMCharacterSelectInfoItemWidget::Refresh()
{
	if (IsValid(InfoHorizontalBox))
	{
	}
}

void UKMCharacterSelectInfoItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMCharacterSelectInfoItemWidget::SetTitleText(const FString& newTitleText)
{
	if (IsValid(TitleTextBlock))
	{
		TitleTextBlock->SetText(FText::FromString(newTitleText));
	}
}

void UKMCharacterSelectInfoItemWidget::SetContentText(const FString& newContentText)
{
	if (IsValid(ContentTextBlock))
	{
		ContentTextBlock->SetText(FText::FromString(newContentText));
	}
}
