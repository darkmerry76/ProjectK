#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMCharacterSelectInfoItemWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMCharacterSelectInfoItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMHorizontalBox> InfoHorizontalBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMTextBlock> TitleTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMTextBlock> ContentTextBlock;

public:
	UKMCharacterSelectInfoItemWidget(const FObjectInitializer& objectInitializer);

	void SetTitleText(const FString& newTitleText);
	void SetContentText(const FString& newContentText);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();
};
