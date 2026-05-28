#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHUD_EmpGaugeWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_EmpGaugeWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMImage* HaloImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* EmpPhaseTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UKMHUD_FloorGaugeWidget* EmpGaugeWidget;
	
	UPROPERTY(Transient)
	class UEMTweener* GaugeTweener;
	
public:
	UKMHUD_EmpGaugeWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();
	void SetEmpPhaseCurr(int32 empPhaseCurr);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSetEmpPhaseCurr(int32 empPhaseCurr);

	UFUNCTION()
	void OnStatChange(UKMCharacterInstance* characterInstance, EKMStatFactorType factorType, float prevValue, float newValue);
};
