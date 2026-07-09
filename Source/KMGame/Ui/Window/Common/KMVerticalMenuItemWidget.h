#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMVerticalMenuItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMVerticalMenuItemPressedDelegate, class UKMVerticalMenuItemWidget*, menuItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMVerticalMenuItemHoverDelegate, class UKMVerticalMenuItemWidget*, menuItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMVerticalMenuItemUnhoverDelegate, class UKMVerticalMenuItemWidget*, menuItemWidget);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMVerticalMenuItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMButton> MenuButton;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMTextBlock> MenuTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FName MenuId = NAME_None;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FLinearColor NormalTextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FLinearColor SelectTextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float NormalFontSize = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredFontSize = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredAnimatinTime = 0.2f;

	FWidgetTransform DefaultTextWidgetTransform;

public:
	float StartAlpha = 0.f;
	float NextAlpha = 0.f;
	float CurrentAnimTime = 0.f;
	float CurrentHoveredAnimationTime = 0.2f;
	float CurrentAlpha = 0.f;
	
public:
	UPROPERTY(BlueprintAssignable)
	FEMVerticalMenuItemPressedDelegate PressedDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMVerticalMenuItemHoverDelegate HoverDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMVerticalMenuItemUnhoverDelegate UnhoverDelegate;

	bool bIsMenualCreated = false;

public:
	UKMVerticalMenuItemWidget(const FObjectInitializer& objectInitializer);

	void SetText(const FString& newText);

	UFUNCTION(BlueprintCallable)
	void HoveredAnimation();

	UFUNCTION(BlueprintCallable)
	void UnhoveredAnimation();

	void SetFontSizeByAlpha(float alpha, bool bEvent = true);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& geometry, float deltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnHoverSelected();
	
	UFUNCTION()
	void OnPressed();

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnUnhovered();
};
