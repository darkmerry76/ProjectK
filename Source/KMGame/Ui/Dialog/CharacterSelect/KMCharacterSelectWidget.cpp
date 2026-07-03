#include "KMCharacterSelectWidget.h"
#include "KMCharacterSelectItemWidget.h"
#include "GameMode/KMGameModeCharacterSelect.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Component/EMButton.h"
#include "UI/Component/EMHorizontalBox.h"

UKMCharacterSelectWidget::UKMCharacterSelectWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(EnterButton))
	{
		EnterButton->OnPressed.AddDynamic(this, &ThisClass::OnEnterPressed);
	}

	Refresh();
}

void UKMCharacterSelectWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(EnterButton))
	{
		EnterButton->OnPressed.RemoveAll(this);
	}
}

void UKMCharacterSelectWidget::Refresh()
{
	if (IsValid(CharacterHorizontalBox))
	{
		FName latestSelectHeroId = NAME_None;
		if (AKMGameModeCharacterSelect* heroSelectGameMode = Cast<AKMGameModeCharacterSelect>(UGameplayStatics::GetGameMode(this)))
		{
			latestSelectHeroId = heroSelectGameMode->LatestHeroTableId;
		}
		
		for(int32 itemIndex = 0; itemIndex < CharacterHorizontalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMCharacterSelectItemWidget* charactrerSelectItem = Cast<UKMCharacterSelectItemWidget>(CharacterHorizontalBox->GetChildAt(itemIndex));
			if (!IsValid(charactrerSelectItem))
			{
				continue;
			}
			if (charactrerSelectItem->CharacterId == latestSelectHeroId)
			{
				charactrerSelectItem->bIsSelected = true;
				OnClicked(charactrerSelectItem);
			}
			else
			{
				charactrerSelectItem->bIsSelected = false;
			}
			
			if (charactrerSelectItem->ClickedDelegate.IsAlreadyBound(this, &ThisClass::OnClicked))
			{
				charactrerSelectItem->ClickedDelegate.RemoveAll(this);
			}
			if (charactrerSelectItem->HoverDelegate.IsAlreadyBound(this, &ThisClass::OnHovered))
			{
				charactrerSelectItem->HoverDelegate.RemoveAll(this);
			}
			if (charactrerSelectItem->UnhoverDelegate.IsAlreadyBound(this, &ThisClass::OnUnhovered))
			{
				charactrerSelectItem->UnhoverDelegate.RemoveAll(this);
			}
			charactrerSelectItem->ClickedDelegate.AddDynamic(this, &ThisClass::OnClicked);
			charactrerSelectItem->HoverDelegate.AddDynamic(this, &ThisClass::OnHovered);
			charactrerSelectItem->UnhoverDelegate.AddDynamic(this, &ThisClass::OnUnhovered);
		}
	}
}

void UKMCharacterSelectWidget::OnClicked_Implementation(UKMCharacterSelectItemWidget* charactrerSelectItem)
{
	if (charactrerSelectItem == PrevSelectedItem)
	{
		return;
	}
	
	if (IsValid(PrevSelectedItem))
	{
		PrevSelectedItem->bIsSelected = false;
		PrevSelectedItem->UnhoveredAnimation();
	}
	charactrerSelectItem->bIsSelected = true;
	charactrerSelectItem->HoveredAnimation();

	PrevSelectedItem = charactrerSelectItem;
}

void UKMCharacterSelectWidget::OnHovered_Implementation(UKMCharacterSelectItemWidget* charactrerSelectItem)
{
	if (IsValid(PrevHoveredItem))
	{
		PrevHoveredItem->UnhoveredAnimation();
	}
	
	charactrerSelectItem->HoveredAnimation();
	PrevHoveredItem = charactrerSelectItem;
}

void UKMCharacterSelectWidget::OnUnhovered_Implementation(UKMCharacterSelectItemWidget* charactrerSelectItem)
{
	if (IsValid(PrevHoveredItem))
	{
		PrevHoveredItem->UnhoveredAnimation();
		PrevHoveredItem = nullptr;
	}
}

void UKMCharacterSelectWidget::OnEnterPressed_Implementation()
{
	
}