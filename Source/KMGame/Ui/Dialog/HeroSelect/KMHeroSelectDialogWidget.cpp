#include "KMHeroSelectDialogWidget.h"
#include "KMHeroSelectWidget.h"
#include "UI/Component/EMButton.h"

UKMHeroSelectDialogWidget::UKMHeroSelectDialogWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHeroSelectDialogWidget::NativeConstruct()
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

void UKMHeroSelectDialogWidget::NativeDestruct()
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

void UKMHeroSelectDialogWidget::OnEnterGame_Implementation()
{
}