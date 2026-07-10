#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMGameHudWindowWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMGameHudWindowWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMHUD_PlayerStatusWidget> PlayerStatus;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMHUD_NPCStatusWidget> NpcStatus;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMHUD_EmpGaugeWidget> EmpGauge;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMHUD_ComboFloatingWidget> ComboFloating;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMHUD_CombatMessageWidget> CombatMessage;
	
public:
	UKMGameHudWindowWidget(const FObjectInitializer& objectInitializer);

	UFUNCTION(BlueprintNativeEvent)
	void ShowHud();

	UFUNCTION(BlueprintNativeEvent)
	void HideHud();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};