#include "KMGameViewportClient.h"
#include "System/EMTickerSubsystem.h"
#include "Ui/Component/KMLoadingWidget.h"
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
}

void UKMGameViewportClient::InitComplete()
{
	Super::InitComplete();

	if (!VersionTextBlock.IsValid())
	{
		SAssignNew(VersionTextBlock, STextBlock)
			.Text_UObject(this, &UKMGameViewportClient::GetBuildInfo);

		SAssignNew(FadeColorBlock, SColorBlock)
			.Color_UObject(this, &ThisClass::GetFadeColor)
			.Visibility_UObject(this, &ThisClass::GetFadeVisibiltiy);

		FSlateFontInfo fontInfo = VersionTextBlock->GetFont();
		fontInfo.Size = 24.f;
		VersionTextBlock->SetFont(fontInfo);
		
		AddViewportWidgetContent(
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				FadeColorBlock.ToSharedRef()
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(20))
			[
				VersionTextBlock.ToSharedRef()
			],100000);
	}

	if (!FadeColorBlock.IsValid())
	{
		SAssignNew(FadeColorBlock, SColorBlock)
		   .Color_UObject(this, &ThisClass::GetFadeColor)
		   .Visibility_UObject(this, &ThisClass::GetFadeVisibiltiy);
		
		AddViewportWidgetContent(FadeColorBlock.ToSharedRef(),90000);
	}
	
	if (!LoadingScreenWidget.IsValid() && IsValid(LoadingScreenWidgetClass))
	{
		LoadingScreenWidgetUMG = CreateWidget<UKMLoadingWidget>(GetGameInstance(), LoadingScreenWidgetClass);
		if (IsValid(LoadingScreenWidgetUMG))
		{
			LoadingScreenWidget = LoadingScreenWidgetUMG->TakeWidget();
			LoadingScreenWidget->SetVisibility(EVisibility::Hidden);
			AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(),80000);
		}
	}
}

void UKMGameViewportClient::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
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
	if (FadeStartAlpha < FadeEndAlpha)
	{
		return EVisibility::Visible;
	}
	else
	{
		return EVisibility::HitTestInvisible;
	}
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

	FadeStartAlpha = startAlpha;
	FadeEndAlpha = endAlpha;

	FadeColor = fadeColor;
	FadeTickerHandle = tickerSubsystem->AddTicker(FBTMTickerDelegate::CreateLambda([this, startAlpha, endAlpha](eTickerEventType eventType, float deltaTime, float eplipseTime, float duration)
	{
		switch (eventType)
		{
		case eTickerEventType::CREATED:
		case eTickerEventType::UPDATED: FadeColor.A = FMath::Lerp(startAlpha, endAlpha, eplipseTime / duration); break;
		case eTickerEventType::REMOVED: FadeColor.A = endAlpha; break;
		default: break;
		}
	}), duration, 0.f);
}

void UKMGameViewportClient::PlayLoadingScreen()
{
	if (LoadingScreenWidget.IsValid())
	{
		FadeColor.A = 0.f;
		LoadingScreenWidget->SetVisibility(EVisibility::Visible);
		LoadingScreenWidget->SetRenderOpacity(1.f);
		LoadingScreenStartTime = GetWorld()->GetTimeSeconds();

		LoadingScreenWidgetUMG->Play();
	}
}
void UKMGameViewportClient::StopLoadingInternal()
{
	if (LoadingScreenWidget.IsValid())
	{
		LoadingScreenWidgetUMG->Stop();
		LoadingScreenWidget->SetVisibility(EVisibility::Hidden);
	}
}

void UKMGameViewportClient::StopLoadingScreenDynamic(FKMLoadingScreenCompleteDynamicDelegate completeDelegate, float minDelyedSeconds)
{
	if (LoadingScreenWidget.IsValid())
	{
		if (GetWorld()->GetTimeSeconds() - LoadingScreenStartTime > minDelyedSeconds)
		{
			StopLoadingInternal();
			completeDelegate.ExecuteIfBound();
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(LoadingScreenTimerHandle, FTimerDelegate::CreateLambda([this, completeDelegate]()
			{
				StopLoadingInternal();
				completeDelegate.ExecuteIfBound();
			}), minDelyedSeconds - (GetWorld()->GetTimeSeconds() - LoadingScreenStartTime), false);
		}
	}
}

void UKMGameViewportClient::StopLoadingScreen(FKMLoadingScreenCompleteDelegate completeDelegate, float minDelyedSeconds)
{
	if (LoadingScreenWidget.IsValid())
	{
		if (GetWorld()->GetTimeSeconds() - LoadingScreenStartTime > minDelyedSeconds)
		{
			StopLoadingInternal();
			completeDelegate.ExecuteIfBound();
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(LoadingScreenTimerHandle, FTimerDelegate::CreateLambda([this, completeDelegate]()
			{
				StopLoadingInternal();
				completeDelegate.ExecuteIfBound();
			}), minDelyedSeconds - (GetWorld()->GetTimeSeconds() - LoadingScreenStartTime), false);
		}
	}
}

bool UKMGameViewportClient::IsPlayingLoadingScreen() const
{
	return LoadingScreenWidget.IsValid();
}

UKMUserWidget* UKMGameViewportClient::GetLoadingScreenWidgetUMG() const
{
	return LoadingScreenWidgetUMG;
}

TSharedPtr<SWidget> UKMGameViewportClient::GetLoadingScreenWidget() const
{
	return LoadingScreenWidget;
}

void UKMGameViewportClient::DetachViewportClient()
{
	RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
	
	VersionTextBlock.Reset();
	FadeColorBlock.Reset();
	LoadingScreenWidget.Reset();
	
	LoadingScreenWidgetUMG = nullptr;
	
	Super::DetachViewportClient();
}