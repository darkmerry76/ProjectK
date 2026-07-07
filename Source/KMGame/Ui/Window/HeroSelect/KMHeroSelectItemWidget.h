#pragma once

#include "CoreMinimal.h"
#include "System/EMTickerSubsystem.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHeroSelectItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMHeroSelectItemClickedDelegate, class UKMHeroSelectItemWidget*, heroSelectItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMHeroSelectItemHoverDelegate, class UKMHeroSelectItemWidget*, heroSelectItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMHeroSelectItemUnhoverDelegate, class UKMHeroSelectItemWidget*, heroSelectItemWidget);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHeroSelectItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMButton* HeroButton;
	
	FEMTickerHandle TickerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredScale = 1.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float SelectedScale = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredAnimatinTime = 0.2f;

	float NextRenderScale = 1.f;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FName HeroId;

public:
	bool bIsSelected = false;
	
	UPROPERTY(BlueprintAssignable)
	FEMHeroSelectItemClickedDelegate ClickedDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMHeroSelectItemHoverDelegate HoverDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMHeroSelectItemUnhoverDelegate UnhoverDelegate;
	
public:
	UKMHeroSelectItemWidget(const FObjectInitializer& objectInitializer);

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

	virtual void NativeTick(const FGeometry& geometry, float deltaTime) override;
};