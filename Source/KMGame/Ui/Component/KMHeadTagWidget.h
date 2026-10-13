#pragma once

#include "CoreMinimal.h"
#include "KMUserWidget.h"
#include "KMHeadTagWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHeadTagWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UEMTextBlock> TitleText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UImage> LockOnImage;
	
public:
	UKMHeadTagWidget(const FObjectInitializer& objectInitializer);
	virtual void NativeConstruct() override;

	void SetVisibileTitleText(ESlateVisibility newVisibility);
	ESlateVisibility GetVisibileTitleText() const;

	void SetVisibileLockOnImage(ESlateVisibility newVisibility);
	ESlateVisibility GetVisibileLockOnImage() const;
};
