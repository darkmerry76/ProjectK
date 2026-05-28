#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHUD_CombatMessageItemWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_CombatMessageItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMWidgetSwitcher* MessageWidgetSwitcher;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMSizeBox* MessageSizeBox;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* MessageTextBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float Duration = 2.f;
	
	EKMCommbatMessageType MessageType = EKMCommbatMessageType::None;

	FTimerHandle TimerHandle;

public:
	UKMHUD_CombatMessageItemWidget(const FObjectInitializer& objectInitializer);
	
	void SetMessage(const FText& messageText);
	void SetMessageSwitch(int32 selectedIndex);
	void SetDuration(float duration);
	void SetFontColor(const FLinearColor& color);
	void SetFontSize(float newFontSize);
	
	void SetMessageType(EKMCommbatMessageType messageType);
	EKMCommbatMessageType GetMessageType() const;
	
	void Refresh();

protected:
	virtual class UWidgetAnimation* GetFadeInAnimation(int32 index) const;
	virtual class UWidgetAnimation* GetFadeOutAnimation(int32 index) const;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnFadeInComplate();

	UFUNCTION()
	void OnFadeOutComplate();
	
	void OnTimerComplate();
};
