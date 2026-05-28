#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHUD_FloorGaugeWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_FloorGaugeWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMHUD_FloorGaugeItemWidget>  FloorGaugeItemWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMHorizontalBox* GaugeHorozontalBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GuageItemCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ItemSizeX = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ItemSizeY = 14;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Slope = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RightMargin = -30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor GaugeColorA = FLinearColor(0.5f, 0.5f, 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor GaugeColorB = FLinearColor(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GaugeColorWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor BackgroundColor = FLinearColor(1.f, 1.f, 1.f, 0.2);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Percent = 0.5f;
	
public:
	UKMHUD_FloorGaugeWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;
	virtual void NativePreConstruct() override;

	void Refresh();

	UFUNCTION(BlueprintCallable)
	void SetPercent(float newPercent);

	UFUNCTION(BlueprintPure)
	float GetPercent() const;
};
