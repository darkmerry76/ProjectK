#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHeroSelectDialogWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHeroSelectDialogWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMHeroSelectWidget> HeroSelectWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMHeroSelectInfoWidget> HeroSelectInfoWidget;
	
public:
	UKMHeroSelectDialogWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnEnterGame();
};