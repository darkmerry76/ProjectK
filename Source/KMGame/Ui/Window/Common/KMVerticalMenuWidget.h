#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMVerticalMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMVerticalMenuItemSelectedDelegate, class UKMVerticalMenuItemWidget*, menuItemWidget);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMVerticalMenuWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMVerticalBox> MenuVerticalBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TSubclassOf<class UKMVerticalMenuItemWidget> MenuItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TArray<FString> MenuItemText;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UKMVerticalMenuItemWidget> PrevSelectMenuItem;
	
public:
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FEMVerticalMenuItemSelectedDelegate SelectedDelegate;

public:
	UKMVerticalMenuWidget(const FObjectInitializer& objectInitializer);

	UFUNCTION(BlueprintNativeEvent)
	void OnMenuPressed(class UKMVerticalMenuItemWidget* verticalMenuItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnMenuHovered(class UKMVerticalMenuItemWidget* verticalMenuItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnMenuUnhovered(class UKMVerticalMenuItemWidget* verticalMenuItem);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();
	void Clear();

	virtual void NativeTick(const FGeometry& myGeometry, float deltaTime) override;

	void SelectedMenu(class UKMVerticalMenuItemWidget* verticalMenuItem);
};
