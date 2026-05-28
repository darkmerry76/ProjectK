#include "KMHUD_FloorGaugeItemWidget.h"
#include "UI/Component/EMImage.h"

UKMHUD_FloorGaugeItemWidget::UKMHUD_FloorGaugeItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

bool UKMHUD_FloorGaugeItemWidget::Initialize()
{
	bool bResult = Super::Initialize();

	return bResult;
}

void UKMHUD_FloorGaugeItemWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!IsDesignTime())
	{
		Refresh();
	}
}

void UKMHUD_FloorGaugeItemWidget::Refresh()
{
	SetFillColorA(FillColorA);
	SetFillColorB(FillColorB);
	SetPercent(Percent);
	SetSlope(Slope);
}

UEMImage* UKMHUD_FloorGaugeItemWidget::GetGaugeImage() const
{
	return GaugeImage;
}

void UKMHUD_FloorGaugeItemWidget::SetFillColorA(FLinearColor newColor)
{
	FillColorA = newColor;
	if (UMaterialInstanceDynamic* materialInstanceDynamic = Cast<UMaterialInstanceDynamic>(GaugeImage->GetDynamicMaterial()))
	{
		materialInstanceDynamic->SetVectorParameterValue(FillColorA_ParameterName, FillColorA);		
	}
}

void UKMHUD_FloorGaugeItemWidget::SetFillColorB(FLinearColor newColor)
{
	FillColorB = newColor;
	if (UMaterialInstanceDynamic* materialInstanceDynamic = Cast<UMaterialInstanceDynamic>(GaugeImage->GetDynamicMaterial()))
	{
		materialInstanceDynamic->SetVectorParameterValue(FillColorB_ParameterName, FillColorB);		
	}
}

void UKMHUD_FloorGaugeItemWidget::SetBackgroundColor(FLinearColor newColor)
{
	BackgroundColor = newColor;
	if (UMaterialInstanceDynamic* materialInstanceDynamic = Cast<UMaterialInstanceDynamic>(GaugeImage->GetDynamicMaterial()))
	{
		materialInstanceDynamic->SetVectorParameterValue(BackgroundColor_ParameterName, BackgroundColor);		
	}
}

void UKMHUD_FloorGaugeItemWidget::SetPercent(float newPercent)
{
	Percent = FMath::Min(newPercent, 1.f);
	if (UMaterialInstanceDynamic* materialInstanceDynamic = Cast<UMaterialInstanceDynamic>(GaugeImage->GetDynamicMaterial()))
	{
		materialInstanceDynamic->SetScalarParameterValue(Percent_ParameterName, Percent);		
	}
}

float UKMHUD_FloorGaugeItemWidget::GetPercent() const
{
	return Percent;
}

void UKMHUD_FloorGaugeItemWidget::SetSlope(float newSlope)
{
	Slope = newSlope;
	if (UMaterialInstanceDynamic* materialInstanceDynamic = Cast<UMaterialInstanceDynamic>(GaugeImage->GetDynamicMaterial()))
	{
		materialInstanceDynamic->SetScalarParameterValue(Slope_ParameterName, newSlope);		
	}
}

float UKMHUD_FloorGaugeItemWidget::GetSlope() const
{
	return Slope;
}