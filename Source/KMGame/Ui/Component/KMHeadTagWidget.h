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

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UEMTweener> GaugeTweener;
	
public:
	UKMHeadTagWidget(const FObjectInitializer& objectInitializer);
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintPure)
	class UEMTextBlock* GetTitleText() const;

	UFUNCTION(BlueprintPure)
	class UImage* GetLockOnImage() const;

	UFUNCTION(BlueprintCallable)
	void SetVisibileTitleText(ESlateVisibility newVisibility);

	UFUNCTION(BlueprintPure)
	ESlateVisibility GetVisibileTitleText() const;

	UFUNCTION(BlueprintCallable)
	void SetVisibileLockOnImage(ESlateVisibility newVisibility);

	UFUNCTION(BlueprintPure)
	ESlateVisibility GetVisibileLockOnImage() const;
};
