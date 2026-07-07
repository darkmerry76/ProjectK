#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHeroSelectWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHeroSelectWidget : public UKMUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMHorizontalBox> HeroHorizontalBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMButton> EnterButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TSubclassOf<class UKMHeroSelectItemWidget> HeroItemClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UKMHeroSelectItemWidget> PrevSelectedItem;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UKMHeroSelectItemWidget> PrevHoveredItem;

	FName DefaultHeroId = TEXT("H_RyuX");
	
public:
	UKMHeroSelectWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnWorldLoadingComplete();

	void Refresh();

	UFUNCTION(BlueprintNativeEvent)
	void OnClicked(class UKMHeroSelectItemWidget* heroSelectItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnHovered(class UKMHeroSelectItemWidget* heroSelectItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnUnhovered(class UKMHeroSelectItemWidget* heroSelectItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnterPressed();

protected:
	void SelectHero(class UKMHeroSelectItemWidget* heroSelectItem);
};