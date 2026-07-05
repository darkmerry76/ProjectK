#include "KMLoadingWidget.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "UI/Component/EMImage.h"

UKMLoadingWidget::UKMLoadingWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

bool UKMLoadingWidget::Initialize()
{
	bool bResult = Super::Initialize();

	if (IsValid(MediaPlayer))
	{
		MediaPlayer->OnMediaOpened.AddDynamic(this, &ThisClass::OnMediaOpened);
		Background->SetVisibility(ESlateVisibility::Hidden);
	}

	return bResult;
}

void UKMLoadingWidget::OnMediaOpened(FString openedUrl)
{
	MediaPlayer->Seek(FTimespan::FromSeconds(SeekSeconds));
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		Background->SetVisibility(ESlateVisibility::Visible);	
	}));
}

void UKMLoadingWidget::Play_Implementation()
{
	if (IsValid(MediaPlayer))
	{
		Background->SetVisibility(ESlateVisibility::Hidden);
		MediaPlayer->OpenSource(MediaSource);
	}
}

void UKMLoadingWidget::Stop_Implementation()
{
	if (IsValid(MediaPlayer))
	{
		Background->SetVisibility(ESlateVisibility::Hidden);
		MediaPlayer->Close();
		if (IsValid(MediaTexture))
		{
			MediaTexture->UpdateResource();
		}
	}
}