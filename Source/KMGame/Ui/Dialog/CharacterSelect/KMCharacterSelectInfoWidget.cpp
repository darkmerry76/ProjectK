#include "KMCharacterSelectInfoWidget.h"
#include "KMCharacterSelectInfoItemWidget.h"
#include "GameMode/KMGameModeCharacterSelect.h"
#include "Kismet/GameplayStatics.h"
#include "Tables/Generated/KMTable_Character_Hero.h"
#include "UI/Component/EMTextBlock.h"
#include "UI/Component/EMVerticalBox.h"

UKMCharacterSelectInfoWidget::UKMCharacterSelectInfoWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCharacterSelectInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AKMGameModeCharacterSelect* heroSelectGameMode = Cast<AKMGameModeCharacterSelect>(UGameplayStatics::GetGameMode(this)))
	{
		heroSelectGameMode->HeroSelectDelegate.AddUObject(this, &ThisClass::SelectHero);
	}

	Refresh();
}

void UKMCharacterSelectInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (AKMGameModeCharacterSelect* heroSelectGameMode = Cast<AKMGameModeCharacterSelect>(UGameplayStatics::GetGameMode(this)))
	{
		heroSelectGameMode->HeroSelectDelegate.RemoveAll(this);
	}
}

void UKMCharacterSelectInfoWidget::Refresh()
{
	SelectHero(CurrentHeroTableId);
}

void UKMCharacterSelectInfoWidget::SelectHero(const FName& heroTableId)
{
	const FKMTable_Character_HeroRow* heroTableRow = FKMTable_Character_HeroRow::FindRowPtr(heroTableId);
	if (!heroTableRow)
	{
		return;
	}

	if (IsValid(HeroNameText))
	{
		HeroNameText->SetText(FText::FromString(heroTableRow->Name));
	}
	if (IsValid(HeroInfoVerticalBox))
	{
		for(int32 itemIndex = 0; itemIndex < HeroInfoVerticalBox->GetChildrenCount(); ++itemIndex)
		{
			UKMCharacterSelectInfoItemWidget* heroSelectInfoItem = Cast<UKMCharacterSelectInfoItemWidget>(HeroInfoVerticalBox->GetChildAt(itemIndex));
			if (!IsValid(heroSelectInfoItem))
			{
				continue;
			}
			if (itemIndex == 0)
			{
				static const UEnum* fightStyleTypeEnum = FindObject<UEnum>(nullptr, TEXT("/Script/KMGame.EKMFightStyleType"), true);
				check(IsValid(fightStyleTypeEnum));

				FText fightStyleDisplayText = fightStyleTypeEnum->GetDisplayNameTextByValue(static_cast<int64>(heroTableRow->FightStyle));
				
				heroSelectInfoItem->SetTitleText(FString::Printf(TEXT("Style / %s"), *fightStyleDisplayText.ToString()));
			}
			else if (itemIndex == 1)
			{
				static const UEnum* coreArmamentType = FindObject<UEnum>(nullptr, TEXT("/Script/KMGame.EKMCoreArmamentType"), true);
				check(IsValid(coreArmamentType));

				FText coreArmamentDisplayText = coreArmamentType->GetDisplayNameTextByValue(static_cast<int64>(heroTableRow->CoreArmament));
				heroSelectInfoItem->SetTitleText(FString::Printf(TEXT("Armament / %s"), *coreArmamentDisplayText.ToString()));
			}
		}
	}
	
	OnHeroSelected(heroTableId);
}