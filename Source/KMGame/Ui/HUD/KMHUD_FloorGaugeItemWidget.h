#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHUD_FloorGaugeItemWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_FloorGaugeItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UEMImage* GaugeImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName FillColorA_ParameterName = FName("FillColorA");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor FillColorA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName FillColorB_ParameterName = FName("FillColorB");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor FillColorB;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BackgroundColor_ParameterName = FName("BackgroundColor");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor BackgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Percent_ParameterName = FName("Percent");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Percent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Slope_ParameterName = FName("Angle");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Slope;

protected:
	virtual void NativePreConstruct() override;
	virtual bool Initialize() override;
	
public:
	UKMHUD_FloorGaugeItemWidget(const FObjectInitializer& objectInitializer);

	void Refresh();
	class UEMImage* GetGaugeImage() const;
	
	void SetFillColorA(FLinearColor newColor);
	void SetFillColorB(FLinearColor newColor);
	void SetBackgroundColor(FLinearColor newColor);

	void SetPercent(float newPercent);
	float GetPercent() const;

	void SetSlope(float newSlope);
	float GetSlope() const;
};
