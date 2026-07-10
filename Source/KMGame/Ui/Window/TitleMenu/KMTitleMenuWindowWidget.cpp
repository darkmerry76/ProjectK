#include "KMTitleMenuWindowWidget.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "System/KMUiSubsystem.h"
#include "Ui/Window/Common/KMVerticalMenuItemWidget.h"

UKMTitleMenuWindowWidget::UKMTitleMenuWindowWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMTitleMenuWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(TitleMenu))
	{
		TitleMenu->SelectedDelegate.AddDynamic(this, &ThisClass::OnSelectedMenu);
	}
	if (IsValid(MediaPlayer) && IsValid(MediaSource))
	{
		MediaPlayer->OnMediaOpened.AddDynamic(this, &ThisClass::OnMediaOpened);
		MediaPlayer->OnEndReached.AddDynamic(this, &ThisClass::OnEndReached);
		MediaPlayer->OpenSource(MediaSource);
	}
}

void UKMTitleMenuWindowWidget::OnMediaOpened(FString openedUrl)
{
	MediaPlayer->SetLooping(true);
	UE_LOG(LogTemp, Warning, TEXT("Duration : %s"), *MediaPlayer->GetDuration().ToString());
}

void UKMTitleMenuWindowWidget::OnEndReached()
{
	
}

void UKMTitleMenuWindowWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(TitleMenu))
	{
		TitleMenu->SelectedDelegate.RemoveAll(this);
	}
}

void UKMTitleMenuWindowWidget::OnSelectedMenu_Implementation(UKMVerticalMenuItemWidget* verticalMenuItem)
{
	if (UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this))
	{
		uiSubsystem->SelectedTitleMenu(verticalMenuItem->MenuId);
	}
}