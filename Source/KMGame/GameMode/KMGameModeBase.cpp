#include "KMGameModeBase.h"
#include "Core/KMWorldSettings.h"
#include "Engine/World.h"
#include "System/KMUiSubsystem.h"
#include "Ui/Component/KMRootWidget.h"

void AKMGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKMGameModeBase::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);

	if (IsValid(StateWidget))
	{
		StateWidget->RemoveFromParent();
	}
}

bool AKMGameModeBase::IsInitMap(const UWorld* otherWorld) const
{
	if (IsValid(otherWorld))
	{
		return false;
	}
	return otherWorld->GetFName() == *InitMap.GetAssetName();
}

void AKMGameModeBase::SpawnStateWidget()
{
	UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this);
	if (!IsValid(uiSubsystem))
	{
		return;
	}

	AKMWorldSettings* worldSettings = Cast<AKMWorldSettings>(GetWorld()->GetWorldSettings());
	if (!IsValid(worldSettings))
	{
		return;
	}
		
	if (IsValid(worldSettings->StateWidgetClass))
	{
		StateWidget = CreateWidget<UKMUserWidget>(GetWorld(), worldSettings->StateWidgetClass);
		uiSubsystem->AttachStateWidget(StateWidget);
	}
}

void AKMGameModeBase::OnWorldLoadingComplete_Implementation()
{
	if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
	{
		playerController->SetInputMode(FInputModeGameAndUI());
	}

	if (UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this))
	{
		uiSubsystem->CreateRoot();
	}
	
	if (bIsAutoAttachStateWidget)
	{
		SpawnStateWidget();
	}
}