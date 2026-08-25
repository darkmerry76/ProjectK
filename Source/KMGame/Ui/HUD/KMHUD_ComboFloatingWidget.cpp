#include "KMHUD_ComboFloatingWidget.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "UI/Component/EMTextBlock.h"

UKMHUD_ComboFloatingWidget::UKMHUD_ComboFloatingWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

bool UKMHUD_ComboFloatingWidget::Initialize()
{
	bool bResult = Super::Initialize();

	if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
	{
		if (AKMCharacter* playerCharactger = Cast<AKMCharacter>(playerController->GetPawn()))
		{
			playerCharactger->GetCharacterInstance()->GetInflictDelegate().AddUObject(this, &ThisClass::OnPlayerInflict);
		}
	}
	
	return bResult;
}

void UKMHUD_ComboFloatingWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
	{
		if (AKMCharacter* playerCharactger = Cast<AKMCharacter>(playerController->GetPawn()))
		{
			playerCharactger->GetCharacterInstance()->GetInflictDelegate().RemoveAll(this);
		}
	}
}

void UKMHUD_ComboFloatingWidget::ShowHitMessage(int32 newHitCount)
{
	ComboCountTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d"), newHitCount)));
	OnShowHitMessage(newHitCount);
}

void UKMHUD_ComboFloatingWidget::OnPlayerInflict(int32 comboCount, UKMGameObjectInstance* victimGameObjectInstance)
{
	ShowHitMessage(comboCount);
}