#include "KMHeroSelectWindowWidget.h"
#include "KMHeroSelectWidget.h"
#include "UI/Component/EMButton.h"

UKMHeroSelectWindowWidget::UKMHeroSelectWindowWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHeroSelectWindowWidget::NativeConstruct()
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

void UKMHeroSelectWindowWidget::NativeDestruct()
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

void UKMHeroSelectWindowWidget::OnEnterGame_Implementation()
{
}