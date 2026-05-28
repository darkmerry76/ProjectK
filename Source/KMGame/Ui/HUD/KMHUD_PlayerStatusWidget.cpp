#include "KMHUD_PlayerStatusWidget.h"
#include "KMHUD_FloorGaugeWidget.h"
#include "Account/KMPlayerAccount.h"
#include "Core/KMGameInstance.h"
#include "GameObject/KMCharacterInstance.h"
#include "System/EMTweener.h"
#include "System/KMGameObjectSubsystem.h"
#include "UI/Component/EMTextBlock.h"

UKMHUD_PlayerStatusWidget::UKMHUD_PlayerStatusWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHUD_PlayerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));
	
	UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
	check(IsValid(authCharacterInstance));
	
	HpGaugeTweener = NewObject<UEMTweener>(this);
	HpGaugeTweener->AddTweenDelegate(FEMTweenDelegate::CreateLambda([this](float prevGauge, float nextGauge, float eplipseTime, float duration)
	{
		if (!IsValid(HpGaugeWidget))
		{
			return;
		}
		float currHpRatio = FMath::Min(eplipseTime / duration, 1.f);
		float currHpPercent = FMath::Lerp(prevGauge, nextGauge, currHpRatio); 
		HpGaugeWidget->SetPercent(currHpPercent);
	}));

	TempoGaugeTweener = NewObject<UEMTweener>(this);
	TempoGaugeTweener->AddTweenDelegate(FEMTweenDelegate::CreateLambda([this](float prevGauge, float nextGauge, float eplipseTime, float duration)
	{
		if (!IsValid(TempoGaugeWidget))
		{
			return;
		}
		float currTempoRatio = FMath::Min(eplipseTime / duration, 1.f);
		float currTempoPercent = FMath::Lerp(prevGauge, nextGauge, currTempoRatio); 
		TempoGaugeWidget->SetPercent(currTempoPercent);
	}));

	authCharacterInstance->GetStatChangeEvent().AddUObject(this, &ThisClass::OnStatChange);
	Refresh();
}

void UKMHUD_PlayerStatusWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	if(IsValid(gameObjectSubsystem))
	{
		UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
		if(IsValid(authCharacterInstance))
		{
			authCharacterInstance->GetStatChangeEvent().RemoveAll(this);
		}
	}
}

void UKMHUD_PlayerStatusWidget::Refresh()
{
	UKMGameInstance* gameInstance = UKMGameInstance::GetGameInstance(this);
	check(IsValid(gameInstance));

	UKMPlayerAccount* playerAccount = gameInstance->GetPlayerAccount();
	check(IsValid(playerAccount))

	NickNameTextBlock->SetText(FText::FromString(playerAccount->BaseInfo.PlayerName));
	LpTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%lld"), playerAccount->BaseInfo.LevelPoint)));
	CoinTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%lld"), playerAccount->BaseInfo.Coin)));
	StatusMessageTextBlock->SetText(FText::FromString(playerAccount->BaseInfo.StatusMessage));
}

void UKMHUD_PlayerStatusWidget::OnStatChange(UKMCharacterInstance* characterInstance, EKMStatFactorType factorType, float prevValue, float newValue)
{
	check(IsValid(characterInstance));

	if (factorType == EKMStatFactorType::TempoCurr)
	{
		if(characterInstance->GetStatModifier()->GetEffectiveStat().GetTempo() <= 0.0001f)
		{
			return;
		}

		float prevTempoPercent = prevValue / characterInstance->GetStatModifier()->GetEffectiveStat().GetTempo();
		float nextTempoPercent = newValue / characterInstance->GetStatModifier()->GetEffectiveStat().GetTempo();
		
		TempoGaugeTweener->Play(prevTempoPercent, nextTempoPercent, 0.2f);
	}
	else if (factorType == EKMStatFactorType::HpCurr)
	{
		check(characterInstance->GetStatModifier()->GetEffectiveStat().GetHp() > 0.f);

		float prevHpPercent = prevValue / characterInstance->GetStatModifier()->GetEffectiveStat().GetHp();
		float nextHpPercent = newValue / characterInstance->GetStatModifier()->GetEffectiveStat().GetHp();
		
		HpGaugeTweener->Play(prevHpPercent, nextHpPercent, 0.2f);
	}
}