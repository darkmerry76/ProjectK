#include "KMNarrativeWidget.h"
#include "UI/Component/EMTextBlock.h"

UKMNarrativeWidget::UKMNarrativeWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

bool UKMNarrativeWidget::Initialize()
{
	bool bResult = Super::Initialize();

	return bResult;
}

void UKMNarrativeWidget::ClearText()
{
	MessageText->SetText(FText::GetEmpty());
	MessageText1->SetText(FText::GetEmpty());
}

void UKMNarrativeWidget::SetMessageText(const FText& text, const FLinearColor& color, bool nessageClear)
{
	if (nessageClear)
	{
		ClearText();
		MessageRow = 0;
	}
	UEMTextBlock* finalTextBlock = MessageText;
	if (MessageRow > 0)
	{
		finalTextBlock = MessageText1;
	}
	if (!IsValid(finalTextBlock))
	{
		return;
	}

	finalTextBlock->SetText(text);
	finalTextBlock->SetColorAndOpacity(color);

	MessageRow++;
}