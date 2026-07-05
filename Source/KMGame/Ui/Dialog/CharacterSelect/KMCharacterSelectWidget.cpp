#include "KMCharacterSelectWidget.h"
#include "KMCharacterSelectItemWidget.h"
#include "GameMode/KMGameModeCharacterSelect.h"
#include "GameObject/KMHeroInstance.h"
#include "Kismet/GameplayStatics.h"
#include "System/KMWorldSubsystem.h"
#include "Tables/Generated/KMTable_Character.h"
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
	if (UKMWorldSubsystem* worldSubsystem = UKMWorldSubsystem::GetWorldSubsystem(this))
	{
		worldSubsystem->LoadingCompleteDelegate.AddUObject(this, &ThisClass::OnWorldLoadingComplete);
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

void UKMCharacterSelectWidget::OnWorldLoadingComplete()
{
}

void UKMCharacterSelectWidget::Refresh()
{
	if (AKMGameModeCharacterSelect* gameMode = Cast<AKMGameModeCharacterSelect>(UGameplayStatics::GetGameMode(this)))
	{
		DefaultHeroId = gameMode->DefaultSelectTableId;
	}
	
	if (IsValid(CharacterHorizontalBox))
	{
		for(int32 itemIndex = 0; itemIndex < CharacterHorizontalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMCharacterSelectItemWidget* charactrerSelectItem = Cast<UKMCharacterSelectItemWidget>(CharacterHorizontalBox->GetChildAt(itemIndex));
			if (!IsValid(charactrerSelectItem))
			{
				continue;
			}
			if (charactrerSelectItem->CharacterId == DefaultHeroId)
			{
				SelectCharacter(charactrerSelectItem);
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
	SelectCharacter(charactrerSelectItem);
}

void UKMCharacterSelectWidget::SelectCharacter(UKMCharacterSelectItemWidget* charactrerSelectItem)
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
	if (AKMGameModeCharacterSelect* heroSelectGameMode = Cast<AKMGameModeCharacterSelect>(UGameplayStatics::GetGameMode(this)))
	{
		heroSelectGameMode->OnEnterGame();
	}	
}