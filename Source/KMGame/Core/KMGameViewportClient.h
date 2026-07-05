#pragma once

#include "CoreMinimal.h"
#include "Core/EMGameViewportClient.h"
#include "System/EMTickerSubsystem.h"
#include "UI/Component/KMUserWidget.h"
#include "KMGameViewportClient.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMGameViewportClient
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_DELEGATE(FKMLoadingScreenCompleteDelegate)
DECLARE_DYNAMIC_DELEGATE(FKMLoadingScreenCompleteDynamicDelegate);

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMGameViewportClient : public UEMGameViewportClient
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UKMLoadingWidget> LoadingScreenWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UKMLoadingWidget> LoadingScreenWidgetUMG;

public:
	virtual void Init(struct FWorldContext& worldContext, UGameInstance* owningGameInstance, bool bCreateNewAudioDevice = true) override;
	virtual void Tick(float deltaTime) override;

	virtual void InitComplete() override; 

	UFUNCTION(BlueprintCallable)
	void PlayFade(float startAlpha, float endAlpha, float duration,  FLinearColor fadeColor = FLinearColor::Black);

	UFUNCTION(BlueprintCallable)
	void PlayLoadingScreen();

	UFUNCTION(BlueprintCallable, DisplayName="StopLoadingScreen")
	void StopLoadingScreenDynamic(FKMLoadingScreenCompleteDynamicDelegate completeDelegate, float minDelyedSeconds = 2.f);
	
	void StopLoadingScreen(FKMLoadingScreenCompleteDelegate completeDelegate, float minDelyedSeconds = 2.f);

	UFUNCTION(BlueprintPure)
	bool IsPlayingLoadingScreen() const;

	UFUNCTION(BlueprintCallable)
	class UKMUserWidget* GetLoadingScreenWidgetUMG() const;

	TSharedPtr<class SWidget> GetLoadingScreenWidget() const;

protected:
	void StopLoadingInternal();

protected:
	FLinearColor GetFadeColor() const;
	EVisibility GetFadeVisibiltiy() const;
	FText GetBuildInfo() const;

	virtual void DetachViewportClient() override;

protected:
	TSharedPtr<class STextBlock> VersionTextBlock;
	TSharedPtr<class SColorBlock> FadeColorBlock;
	TSharedPtr<class SWidget> LoadingScreenWidget;

	FLinearColor FadeColor;
	FEMTickerHandle FadeTickerHandle;

	float LoadingScreenStartTime = 0.f;

	FTimerHandle LoadingScreenTimerHandle;

	float FadeStartAlpha = 0.f;
	float FadeEndAlpha = 0.f;
};
