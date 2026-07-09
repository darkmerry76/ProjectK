#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "Ui/Window/Common/KMVerticalMenuWidget.h"
#include "KMTitleMenuWindowWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMTitleMenuWindowWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UKMVerticalMenuWidget> TitleMenu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UMediaPlayer> MediaPlayer;

public:
	UKMTitleMenuWindowWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnSelectedMenu(class UKMVerticalMenuItemWidget* verticalMenuItem);
};