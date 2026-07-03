#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMCharacterSelectDialogWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMCharacterSelectDialogWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMCharacterSelectWidget> HeroSelectWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMCharacterSelectInfoWidget> HeroSelectInfoWidget;
	
public:
	UKMCharacterSelectDialogWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnEnterGame();
};
