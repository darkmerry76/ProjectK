#include "KMPrologueWindow.h"
#include "System/EMTickerSubsystem.h"
#include "Tables/Generated/KMTable_Narrative_Prologue.h"
#include "UI/Component/EMBorder.h"
#include "UI/Component/EMRichTextBlock.h"
#include "UI/Component/EMScrollBox.h"

void UKMPrologueWindowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UKMPrologueWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PrologueTableRow = FKMTable_Narrative_PrologueRow::FindRowPtr(PrologueTableId);
	if (IsValid(RichTextBlock) && PrologueTableRow)
	{
		RichTextBlock->SetText(FText::FromString(PrologueTableRow->Text));
	}

	UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
	check(IsValid(tickerSubsystem));
		
	if (IsValid(ScrollBox) && PrologueTableRow)
	{
		if (PrologueTableRow->FadeInTime> 0.f)
		{
			tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateUObject(this, &ThisClass::OnFadeinAnimation), PrologueTableRow->FadeInTime , 0.f);
		}
		
		if (PrologueTableRow->Duration + PrologueTableRow->StartDelay  > 0.f)
		{
			ScrollTickerHandle = tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateUObject(
				this, &ThisClass::OnScrollAnimation), PrologueTableRow->Duration, PrologueTableRow->StartDelay * -1.f);
		}
		else
		{
			tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateUObject(this, &ThisClass::OnFadeoutAnimation), 1.f, 0.f);
		}
	}
}

void UKMPrologueWindowWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMPrologueWindowWidget::OnScrollAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration)
{
	float alpha = FMath::Clamp(eplipseTime / duration,0.0f,1.0f);
	float maxOffset = ScrollBox->GetScrollOffsetOfEnd();
	ScrollBox->SetScrollOffset(maxOffset * alpha);
	if (eventType == eTickerEventType::REMOVED)
	{
		if (IsValid(Border) && PrologueTableRow && PrologueTableRow->FadeOutTime > 0.f)
		{
			UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
			check(IsValid(tickerSubsystem));

			tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateUObject(this, &ThisClass::OnFadeoutAnimation), PrologueTableRow->FadeOutTime, 0.f);
		}
	}
}

void UKMPrologueWindowWidget::OnFadeinAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration)
{
	Border->SetRenderOpacity(eplipseTime / duration);
}

void UKMPrologueWindowWidget::OnFadeoutAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration)
{
	Border->SetRenderOpacity(1.f - (eplipseTime / duration));
	if(eventType == eTickerEventType::REMOVED)
	{
		RemoveFromParent();
	}
}