#include "KMHUD_NPCStatusWidget.h"
#include "KMHUD_FloorGaugeWidget.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/KMMonsterInstance.h"
#include "System/EMTweener.h"
#include "UI/Component/EMTextBlock.h"

UKMHUD_NPCStatusWidget::UKMHUD_NPCStatusWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHUD_NPCStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UKMCharacterInstance::GetGlobalStatChangeEvent().AddUObject(this, &UKMHUD_NPCStatusWidget::OnStatChange);

	GaugeTweener = NewObject<UEMTweener>(this);
	GaugeTweener->AddTweenDelegate(FEMTweenDelegate::CreateLambda([this](float prevGauge, float nextGauge, float eplipseTime, float duration)
	{
		if (!IsValid(CurrHpGuageWidget))
		{
			return;
		}
		float currHpRatio = FMath::Min(eplipseTime / 0.2f, 1.f);
		float currHpPercent = FMath::Lerp(prevGauge, nextGauge, currHpRatio); 
		CurrHpGuageWidget->SetPercent(currHpPercent);
	}));

	GaugeTweener->AddTweenDelegate(FEMTweenDelegate::CreateLambda([this](float prevGauge, float nextGauge, float eplipseTime, float duration)
	{
		if (!IsValid(PrevHpGuageWidget))
		{
			return;
		}

		float prevHpRatio = 1.f;
		if (nextGauge > 0.f)
		{
			prevHpRatio = FMath::Clamp((eplipseTime - (duration - 0.2f)) / 0.2f, 0.f, 1.f);
		}
		float prevHpPercent = FMath::Lerp(prevGauge, nextGauge, prevHpRatio); 
		PrevHpGuageWidget->SetPercent(prevHpPercent);
	}));
}

void UKMHUD_NPCStatusWidget::NativeDestruct()
{
	Super::NativeDestruct();
	UKMCharacterInstance::GetGlobalStatChangeEvent().RemoveAll(this);
	if (IsValid(GaugeTweener))
	{
		GaugeTweener->StopAnimation();
	}
}

void UKMHUD_NPCStatusWidget::OnStatChange(UKMGameObjectInstance* gameObjectInstance, EKMStatFactorType factorType, float prevValue, float newValue)
{
	check(IsValid(gameObjectInstance));

	if (factorType != EKMStatFactorType::HpCurr)
	{
		return;
	}
	
	check(gameObjectInstance->GetStatModifier()->GetEffectiveStat().GetHp() > 0.f);

	float prevHpPercent = prevValue / gameObjectInstance->GetStatModifier()->GetEffectiveStat().GetHp();
	float nextHpPercent = newValue / gameObjectInstance->GetStatModifier()->GetEffectiveStat().GetHp();
	
	GaugeTweener->Play(prevHpPercent, nextHpPercent, 1.2f);
	NameTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%s"), *gameObjectInstance->GetObjectName())));

	OnShowHp(gameObjectInstance, newValue, gameObjectInstance->GetStatModifier()->GetEffectiveStat().GetHp());
}