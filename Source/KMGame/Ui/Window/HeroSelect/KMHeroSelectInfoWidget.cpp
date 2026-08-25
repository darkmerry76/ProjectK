#include "KMHeroSelectInfoWidget.h"
#include "KMHeroSelectInfoItemWidget.h"
#include "GameMode/KMGameModeHeroSelect.h"
#include "Kismet/GameplayStatics.h"
#include "Tables/Generated/KMTable_Object_Character_Hero.h"
#include "UI/Component/EMTextBlock.h"
#include "UI/Component/EMVerticalBox.h"

UKMHeroSelectInfoWidget::UKMHeroSelectInfoWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHeroSelectInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AKMGameModeHeroSelect* heroSelectGameMode = Cast<AKMGameModeHeroSelect>(UGameplayStatics::GetGameMode(this)))
	{
		heroSelectGameMode->HeroSelectDelegate.AddUObject(this, &ThisClass::SelectHero);
	}

	Refresh();
}

void UKMHeroSelectInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (AKMGameModeHeroSelect* heroSelectGameMode = Cast<AKMGameModeHeroSelect>(UGameplayStatics::GetGameMode(this)))
	{
		heroSelectGameMode->HeroSelectDelegate.RemoveAll(this);
	}
}

void UKMHeroSelectInfoWidget::Refresh()
{
	SelectHero(CurrentHeroTableId);
}

void UKMHeroSelectInfoWidget::SelectHero(const FName& heroTableId)
{
	const FKMTable_Object_Character_HeroRow* heroTableRow = FKMTable_Object_Character_HeroRow::FindRowPtr(heroTableId);
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
			UKMHeroSelectInfoItemWidget* heroSelectInfoItem = Cast<UKMHeroSelectInfoItemWidget>(HeroInfoVerticalBox->GetChildAt(itemIndex));
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