#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "Stat/KMSecondaryBaseStat.h"
#include "KMHUD_PlayerStatusWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_PlayerStatusWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMImage* CharacterImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UKMHUD_FloorGaugeWidget* HpGaugeWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* NickNameTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* LpTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* CoinTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* StatusMessageTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UKMHUD_FloorGaugeWidget* TempoGaugeWidget;

	UPROPERTY(Transient)
	class UEMTweener* HpGaugeTweener;
	
	UPROPERTY(Transient)
	class UEMTweener* TempoGaugeTweener;

public:
	UKMHUD_PlayerStatusWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();

	void OnStatChange(class UKMCharacterInstance* characterInstance, EKMStatFactorType factorType, float prevValue, float newValue);
};
