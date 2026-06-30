#pragma once

#include "CoreMinimal.h"
#include "System/EMTickerSubsystem.h"
#include "UI/Component/KMUserWidget.h"
#include "KMTitleMenuItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMTitleMenuItemClickedDelegate, class UKMTitleMenuItemWidget*, menuItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMTitleMenuItemHoverDelegate, class UKMTitleMenuItemWidget*, menuItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMTitleMenuItemUnhoverDelegate, class UKMTitleMenuItemWidget*, menuItemWidget);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMTitleMenuItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMButton> MenuButton;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMTextBlock> MenuText;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float NormalFontSize = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredFontSize = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredAnimatinTime = 0.2f;

	FWidgetTransform DefaultTextWidgetTransform;
	FEMTickerHandle TickerHandle;
	
public:
	UPROPERTY(BlueprintAssignable)
	FEMTitleMenuItemClickedDelegate ClickedDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMTitleMenuItemHoverDelegate HoverDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMTitleMenuItemUnhoverDelegate UnhoverDelegate;

	bool bIsMenualCreated = false;

public:
	UKMTitleMenuItemWidget(const FObjectInitializer& objectInitializer);

	void SetText(const FString& newText);

	UFUNCTION(BlueprintCallable)
	void HoveredAnimation();

	UFUNCTION(BlueprintCallable)
	void UnhoveredAnimation();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnClicked();

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnUnhovered();

	void OnHoveredAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration);
	void OnUnhoveredAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration);

	void SetFontSizeByAlpha(float alpha);
};
