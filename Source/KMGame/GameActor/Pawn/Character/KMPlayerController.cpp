#include "KMPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "System/KMUiSubsystem.h"

void AKMPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AKMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void AKMPlayerController::OnEscapePressed()
{
	Super::OnEscapePressed();
}