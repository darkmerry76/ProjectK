#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "Stat/KMSecondaryBaseStat.h"
#include "KMHUD_NPCStatusWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_NPCStatusWidget : public UKMUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UKMHUD_FloorGaugeWidget* PrevHpGuageWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UKMHUD_FloorGaugeWidget* CurrHpGuageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* NameTextBlock;
	
	UPROPERTY(Transient)
	class UEMTweener* GaugeTweener;
	
public:
	UKMHUD_NPCStatusWidget(const FObjectInitializer& objectInitializer);

protected:
	void OnStatChange(class UKMCharacterInstance* characterInstance, EKMStatFactorType factorType, float prevValue, float newValue);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowHp(class UKMCharacterInstance* characterInstance, float currHp, float maxHp);
};
