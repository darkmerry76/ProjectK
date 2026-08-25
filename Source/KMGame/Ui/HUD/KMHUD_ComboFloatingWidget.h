#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHUD_ComboFloatingWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_ComboFloatingWidget : public UKMUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMCanvasPanel* FloatingCanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* ComboCountTextBlock;

public:
	UKMHUD_ComboFloatingWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void ShowHitMessage(int32 newHitCount);

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowHitMessage(int32 newHitCount);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHideHitMessage();

protected:
	void OnPlayerInflict(int32 comboCount, class UKMGameObjectInstance* victimGameObjectInstance);
};
