#pragma once

#include "CoreMinimal.h"
#include "KMUserWidget.h"
#include "KMLoadingWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMLoadingWidget : public UKMUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMediaPlayer> MediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMediaSource> MediaSource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMediaTexture> MediaTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double SeekSeconds = 0.f;;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UEMImage> Background;

public:
	UFUNCTION(BlueprintNativeEvent)
	void Play();

	UFUNCTION(BlueprintNativeEvent)
	void Stop();

protected:
	UFUNCTION()
	void OnMediaOpened(FString openedUrl);
	
public:
	UKMLoadingWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;
};
