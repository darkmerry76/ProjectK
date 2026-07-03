#include "KMGameViewportClient.h"

#include "System/EMTickerSubsystem.h"
#include "Util/KMUtil.h"
#include "Widgets/Colors/SColorBlock.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMGameViewportClient
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMGameViewportClient::UKMGameViewportClient(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	FadeColor = FLinearColor(0.f, 0.f, 0.f, 0.f);
}

void UKMGameViewportClient::Init(FWorldContext& worldContext, UGameInstance* owningGameInstance, bool bCreateNewAudioDevice)
{
	Super::Init(worldContext, owningGameInstance, bCreateNewAudioDevice);

	SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
}

void UKMGameViewportClient::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (!VersionText.IsValid())
	{
		VersionText =
			SNew(STextBlock)
			.Text_UObject(this, &UKMGameViewportClient::GetBuildInfo);

		FSlateFontInfo fontInfo = VersionText->GetFont();
		fontInfo.Size = 24.f;
		VersionText->SetFont(fontInfo);
		
		AddViewportWidgetContent(
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(20))
			[
				VersionText.ToSharedRef()
			], 100000
		);
	}
	if (!FadeColorBlock.IsValid())
	{
		 SAssignNew(FadeColorBlock, SColorBlock)
			.Color_UObject(this, &ThisClass::GetFadeColor)
			.Visibility_UObject(this, &ThisClass::GetFadeVisibiltiy);
		
		AddViewportWidgetContent(FadeColorBlock.ToSharedRef(),90000);
	}
}

FText UKMGameViewportClient::GetBuildInfo() const
{
	return FText::FromString(UKMUtil::GetBuildInfo(this));
}

FLinearColor UKMGameViewportClient::GetFadeColor() const
{
	return FadeColor;
}

EVisibility UKMGameViewportClient::GetFadeVisibiltiy() const
{
	if (FadeColor.A <= 0.00001f)
	{
		return EVisibility::Hidden;
	}
	return EVisibility::Visible;
}

void UKMGameViewportClient::PlayFade(float startAlpha, float endAlpha, float duration, FLinearColor fadeColor)
{
	UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
	if (!IsValid(tickerSubsystem))
	{
		return;
	}
	if (FadeTickerHandle.IsValid())
	{
		tickerSubsystem->RemoveTicker(FadeTickerHandle);
	}

	FadeColor = fadeColor;
	FadeTickerHandle = tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateLambda([this, startAlpha, endAlpha](eTickerEventType eventType, float deltaTime, float eplipseTime, float duration)
	{
		switch (eventType)
		{
		case eTickerEventType::CREATED: FadeColor.A = startAlpha;
		case eTickerEventType::UPDATED: FadeColor.A = FMath::Lerp(startAlpha, endAlpha, eplipseTime / duration); break;
		case eTickerEventType::REMOVED: FadeColor.A = endAlpha; break;
		default: break;
		}
	}), duration, 0.f);
}