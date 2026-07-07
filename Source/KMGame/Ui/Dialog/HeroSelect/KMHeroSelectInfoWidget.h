#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHeroSelectInfoWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHeroSelectInfoWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMCanvasPanel> HeroInfoCanvasPanel;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMVerticalBox> HeroInfoVerticalBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMTextBlock> HeroNameText;

	FName CurrentHeroTableId = TEXT("H_RyuX");

public:
	UKMHeroSelectInfoWidget(const FObjectInitializer& objectInitializer);

	void SelectHero(const FName& heroTableId);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHeroSelected(const FName& heroTableId);
};