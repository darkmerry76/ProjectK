#include "KMGameModeBase.h"
#include "Core/KMWorldSettings.h"
#include "Engine/World.h"
#include "Ui/Component/KMRootWidget.h"

void AKMGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKMGameModeBase::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);

	if (IsValid(RootWidget))
	{
		RootWidget->RemoveFromParent();
	}
}

bool AKMGameModeBase::IsInitMap(const UWorld* otherWorld) const
{
	if (IsValid(otherWorld) == false)
	{
		return false;
	}
	return otherWorld->GetFName() == *InitMap.GetAssetName();
}

void AKMGameModeBase::OnWorldLoadingComplete_Implementation()
{
	if (AKMWorldSettings* worldSettings = Cast<AKMWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		if (IsValid(worldSettings->RootWidgetClass))
		{
			RootWidget = CreateWidget<UKMUserWidget>(GetWorld(), worldSettings->RootWidgetClass);
			if (IsValid(RootWidget))
			{
				RootWidget->AddToViewport(0);
			}
		}
	}
}