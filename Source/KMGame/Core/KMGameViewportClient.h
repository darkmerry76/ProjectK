#pragma once

#include "CoreMinimal.h"
#include "System/EMTickerSubsystem.h"
#include "KMGameViewportClient.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMGameViewportClient
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS()
class KMGAME_API UKMGameViewportClient : public UGameViewportClient
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Init(struct FWorldContext& worldContext, UGameInstance* owningGameInstance, bool bCreateNewAudioDevice = true) override;
	virtual void Tick(float deltaTime) override;

	void PlayFade(float startAlpha, float endAlpha, float duration,  FLinearColor fadeColor = FLinearColor::Black);
	
protected:
	FLinearColor GetFadeColor() const;
	EVisibility GetFadeVisibiltiy() const;
	FText GetBuildInfo() const;

protected:
	TSharedPtr<class STextBlock> VersionText;
	TSharedPtr<class SColorBlock> FadeColorBlock;

	FLinearColor FadeColor;
	FEMTickerHandle FadeTickerHandle;
};
