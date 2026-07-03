#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMCharacterSelectInfoWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMCharacterSelectInfoWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMCanvasPanel> HeroInfoCanvasPanel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMVerticalBox> HeroInfoVerticalBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMTextBlock> HeroNameText;

	FName CurrentHeroTableId = TEXT("H_RyuX");

public:
	UKMCharacterSelectInfoWidget(const FObjectInitializer& objectInitializer);

	void SelectHero(const FName& heroTableId);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHeroSelected(const FName& heroTableId);
};
