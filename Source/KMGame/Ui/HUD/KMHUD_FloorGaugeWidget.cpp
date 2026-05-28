#include "KMHUD_FloorGaugeWidget.h"
#include "KMHUD_FloorGaugeItemWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "UI/Component/EMHorizontalBox.h"
#include "UI/Component/EMImage.h"

UKMHUD_FloorGaugeWidget::UKMHUD_FloorGaugeWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHUD_FloorGaugeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Refresh();
}

void UKMHUD_FloorGaugeWidget::Refresh()
{
	GaugeHorozontalBox->ClearChildren();

	for (int32 itemCount = 0; itemCount < GuageItemCount; ++itemCount)
	{
		UKMHUD_FloorGaugeItemWidget* newGaugeItemWidget = CreateWidget<UKMHUD_FloorGaugeItemWidget>(this, FloorGaugeItemWidgetClass);
		if(!IsValid(newGaugeItemWidget))
		{
			continue;
		}

		FSlateBrush gaugeBrush = newGaugeItemWidget->GetGaugeImage()->GetBrush();

		UE::Slate::FDeprecateVector2DResult newBrushSize(ItemSizeX, ItemSizeY);
		gaugeBrush.SetImageSize(newBrushSize);
		newGaugeItemWidget->GetGaugeImage()->SetBrush(gaugeBrush);
		newGaugeItemWidget->DesignTimeSize = newBrushSize;
		newGaugeItemWidget->SetSlope(Slope);
		newGaugeItemWidget->SetBackgroundColor(BackgroundColor);
	
		UHorizontalBoxSlot* newHorizontalBoxSlot = GaugeHorozontalBox->AddChildToHorizontalBox(newGaugeItemWidget);
		if(!IsValid(newHorizontalBoxSlot))
		{
			continue;
		}

		FSlateChildSize childSize = newHorizontalBoxSlot->GetSize();
		childSize.SizeRule = ESlateSizeRule::Automatic;
		newHorizontalBoxSlot->SetSize(childSize);

		FMargin childPadding = newHorizontalBoxSlot->GetPadding();
		childPadding.Right = RightMargin;
		newHorizontalBoxSlot->SetPadding(childPadding);
	}
	SetPercent(Percent);
}

bool UKMHUD_FloorGaugeWidget::Initialize()
{
	bool bResult = Super::Initialize();

	return bResult;
}

void UKMHUD_FloorGaugeWidget::SetPercent(float newPercent)
{
	float maxPercentCount = GaugeHorozontalBox->GetChildrenCount() * newPercent;
	
	for (int32 itemIndex = 0; itemIndex < GaugeHorozontalBox->GetChildrenCount(); ++itemIndex, maxPercentCount -= 1.f)
	{
		float gaugeColorRatioA = FMath::Pow(static_cast<float>(itemIndex) / static_cast<float>(GaugeHorozontalBox->GetChildrenCount()), GaugeColorWeight);
		float gaugeColorRatioB = FMath::Pow(static_cast<float>(itemIndex + 1) / static_cast<float>(GaugeHorozontalBox->GetChildrenCount()), GaugeColorWeight);
		
		UKMHUD_FloorGaugeItemWidget* gaugeItemWidget = Cast<UKMHUD_FloorGaugeItemWidget>(GaugeHorozontalBox->GetChildAt(itemIndex));
		if (!IsValid(gaugeItemWidget))
		{
			continue;
		}
		
		gaugeItemWidget->SetFillColorA(FMath::Lerp(GaugeColorA, GaugeColorB, gaugeColorRatioA));
		gaugeItemWidget->SetFillColorB(FMath::Lerp(GaugeColorA, GaugeColorB, gaugeColorRatioB));
		gaugeItemWidget->SetPercent(maxPercentCount);	
	}
}

float UKMHUD_FloorGaugeWidget::GetPercent() const
{
	return Percent;
}
