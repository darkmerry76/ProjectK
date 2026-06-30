#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMTitleMenuWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMTitleMenuWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMVerticalBox> MenuVerticalBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TSubclassOf<class UKMTitleMenuItemWidget> TitleMenuItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TArray<FString> MenuItemText;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UKMTitleMenuItemWidget> PrevSelectMenuItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FMargin ItemPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TEnumAsByte<ESlateSizeRule::Type> SizeRule = ESlateSizeRule::Fill;
	
public:
	UKMTitleMenuWidget(const FObjectInitializer& objectInitializer);

	UFUNCTION(BlueprintNativeEvent)
	void OnMenuClicked(class UKMTitleMenuItemWidget* titleMenuItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnMenuHovered(class UKMTitleMenuItemWidget* titleMenuItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnMenuUnhovered(class UKMTitleMenuItemWidget* titleMenuItem);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();
	void Clear();

	virtual void NativeTick(const FGeometry& myGeometry, float deltaTime) override;
};
