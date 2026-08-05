#include "KMHUD_SkillMessageItemWidget.h"
#include "UI/Component/EMTextBlock.h"
#include "UI/Component/EMWidgetSwitcher.h"

UKMHUD_SkillMessageItemWidget::UKMHUD_SkillMessageItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHUD_SkillMessageItemWidget::SetMessage(const FText& messageText)
{
	MessageTextBlock->SetText(messageText);
}

void UKMHUD_SkillMessageItemWidget::SetFontColor(const FLinearColor& color)
{
	FSlateColor fontColor = MessageTextBlock->GetColorAndOpacity();
	fontColor = color;
	MessageTextBlock->SetColorAndOpacity(fontColor);
}

void UKMHUD_SkillMessageItemWidget::SetFontSize(float newFontSize)
{
	FSlateFontInfo fontInfo = MessageTextBlock->GetFont();
	fontInfo.Size = newFontSize;
	MessageTextBlock->SetFont(fontInfo);
}

void UKMHUD_SkillMessageItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Refresh();
}

void UKMHUD_SkillMessageItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMHUD_SkillMessageItemWidget::Refresh()
{
}