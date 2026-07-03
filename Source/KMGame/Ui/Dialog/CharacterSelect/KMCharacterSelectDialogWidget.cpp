#include "KMCharacterSelectDialogWidget.h"
#include "KMCharacterSelectWidget.h"
#include "UI/Component/EMButton.h"

UKMCharacterSelectDialogWidget::UKMCharacterSelectDialogWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCharacterSelectDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HeroSelectWidget))
	{
		if (IsValid(HeroSelectWidget->EnterButton))
		{
			HeroSelectWidget->EnterButton->OnPressed.AddDynamic(this, &ThisClass::OnEnterGame);
		}
	}
}

void UKMCharacterSelectDialogWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(HeroSelectWidget))
	{
		if (IsValid(HeroSelectWidget->EnterButton))
		{
			HeroSelectWidget->EnterButton->OnPressed.RemoveAll(this);
		}
	}
}

void UKMCharacterSelectDialogWidget::OnEnterGame_Implementation()
{
}