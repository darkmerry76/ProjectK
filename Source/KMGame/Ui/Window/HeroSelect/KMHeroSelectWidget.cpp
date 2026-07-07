#include "KMHeroSelectWidget.h"
#include "KMHeroSelectItemWidget.h"
#include "GameMode/KMGameModeHeroSelect.h"
#include "Kismet/GameplayStatics.h"
#include "System/KMWorldSubsystem.h"
#include "UI/Component/EMButton.h"
#include "UI/Component/EMHorizontalBox.h"

UKMHeroSelectWidget::UKMHeroSelectWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHeroSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(EnterButton))
	{
		EnterButton->OnPressed.AddDynamic(this, &ThisClass::OnEnterPressed);
	}
	if (UKMWorldSubsystem* worldSubsystem = UKMWorldSubsystem::GetWorldSubsystem(this))
	{
		worldSubsystem->LoadingCompleteDelegate.AddUObject(this, &ThisClass::OnWorldLoadingComplete);
	}

	Refresh();
}

void UKMHeroSelectWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(EnterButton))
	{
		EnterButton->OnPressed.RemoveAll(this);
	}
}

void UKMHeroSelectWidget::OnWorldLoadingComplete()
{
}

void UKMHeroSelectWidget::Refresh()
{
	if (AKMGameModeHeroSelect* heroSelectGameMode = Cast<AKMGameModeHeroSelect>(UGameplayStatics::GetGameMode(this)))
	{
		DefaultHeroId = heroSelectGameMode->DefaultSelectTableId;
	}
	
	if (IsValid(HeroHorizontalBox))
	{
		for(int32 itemIndex = 0; itemIndex < HeroHorizontalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMHeroSelectItemWidget* heroSelectItem = Cast<UKMHeroSelectItemWidget>(HeroHorizontalBox->GetChildAt(itemIndex));
			if (!IsValid(heroSelectItem))
			{
				continue;
			}
			if (heroSelectItem->HeroId == DefaultHeroId)
			{
				SelectHero(heroSelectItem);
			}
			if (heroSelectItem->ClickedDelegate.IsAlreadyBound(this, &ThisClass::OnClicked))
			{
				heroSelectItem->ClickedDelegate.RemoveAll(this);
			}
			if (heroSelectItem->HoverDelegate.IsAlreadyBound(this, &ThisClass::OnHovered))
			{
				heroSelectItem->HoverDelegate.RemoveAll(this);
			}
			if (heroSelectItem->UnhoverDelegate.IsAlreadyBound(this, &ThisClass::OnUnhovered))
			{
				heroSelectItem->UnhoverDelegate.RemoveAll(this);
			}
			heroSelectItem->ClickedDelegate.AddDynamic(this, &ThisClass::OnClicked);
			heroSelectItem->HoverDelegate.AddDynamic(this, &ThisClass::OnHovered);
			heroSelectItem->UnhoverDelegate.AddDynamic(this, &ThisClass::OnUnhovered);
		}
	}
}

void UKMHeroSelectWidget::OnClicked_Implementation(UKMHeroSelectItemWidget* heroSelectItem)
{
	SelectHero(heroSelectItem);
}

void UKMHeroSelectWidget::SelectHero(UKMHeroSelectItemWidget* heroSelectItem)
{
	if (heroSelectItem == PrevSelectedItem)
	{
		return;
	}
	
	if (IsValid(PrevSelectedItem))
	{
		PrevSelectedItem->bIsSelected = false;
		PrevSelectedItem->UnhoveredAnimation();
	}
	heroSelectItem->bIsSelected = true;
	heroSelectItem->HoveredAnimation();

	PrevSelectedItem = heroSelectItem;
}

void UKMHeroSelectWidget::OnHovered_Implementation(UKMHeroSelectItemWidget* heroSelectItem)
{
	if (IsValid(PrevHoveredItem))
	{
		PrevHoveredItem->UnhoveredAnimation();
	}
	
	heroSelectItem->HoveredAnimation();
	PrevHoveredItem = heroSelectItem;
}

void UKMHeroSelectWidget::OnUnhovered_Implementation(UKMHeroSelectItemWidget* heroSelectItem)
{
	if (IsValid(PrevHoveredItem))
	{
		PrevHoveredItem->UnhoveredAnimation();
		PrevHoveredItem = nullptr;
	}
}

void UKMHeroSelectWidget::OnEnterPressed_Implementation()
{
	if (AKMGameModeHeroSelect* heroSelectGameMode = Cast<AKMGameModeHeroSelect>(UGameplayStatics::GetGameMode(this)))
	{
		heroSelectGameMode->OnEnterGame();
	}	
}