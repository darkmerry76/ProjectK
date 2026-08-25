#include "KMInputPlayerController.h"
#include "System/KMUiSubsystem.h"

void AKMInputPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AKMInputPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		InputComponent->BindKey(
			EKeys::Escape,
			IE_Pressed,
			this,
			&ThisClass::OnEscapePressed);
	}
}

void AKMInputPlayerController::OnEscapePressed()
{
	if (UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this))
	{
		uiSubsystem->HandleEscape();
	}
}