#include "KMHUD_EmpGaugeWidget.h"
#include "KMHUD_FloorGaugeWidget.h"
#include "Character/KMCharacter.h"
#include "Core/EMDefine.h"
#include "System/EMTweener.h"
#include "System/KMGameObjectSubsystem.h"
#include "UI/Component/EMTextBlock.h"

UKMHUD_EmpGaugeWidget::UKMHUD_EmpGaugeWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHUD_EmpGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));
	
	UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
	check(IsValid(authCharacterInstance));
	
	GaugeTweener = NewObject<UEMTweener>(this);
	GaugeTweener->AddTweenDelegate(FEMTweenDelegate::CreateLambda([this](float prevGauge, float nextGauge, float eplipseTime, float duration)
	{
		if (!IsValid(EmpGaugeWidget))
		{
			return;
		}
		float currEmpRatio = FMath::Min(eplipseTime / duration, 1.f);
		float currEmpPercent = FMath::Lerp(prevGauge, nextGauge, currEmpRatio); 
		EmpGaugeWidget->SetPercent(currEmpPercent);
	}));

	authCharacterInstance->GetStatChangeEvent().AddUObject(this, &ThisClass::OnStatChange);
	Refresh();
}

void UKMHUD_EmpGaugeWidget::NativeDestruct()
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

void UKMHUD_EmpGaugeWidget::Refresh()
{
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
	check(IsValid(authCharacterInstance));

	FKMSecondaryStat& secondaryStat = authCharacterInstance->GetStatModifier()->GetEffectiveStat();
	
	EmpGaugeWidget->SetPercent(secondaryStat.GetEmpCurr() / secondaryStat.GetEmp());
	SetEmpPhaseCurr(secondaryStat.GetEmpPhaseCurr());
}

void UKMHUD_EmpGaugeWidget::SetEmpPhaseCurr(int32 empPhaseCurr)
{
	EmpPhaseTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d"), empPhaseCurr)));

	OnSetEmpPhaseCurr(empPhaseCurr);
}

void UKMHUD_EmpGaugeWidget::OnStatChange(UKMCharacterInstance* characterInstance, EKMStatFactorType factorType, float prevValue, float newValue)
{
	check(IsValid(characterInstance));

	if (factorType == EKMStatFactorType::EmpCurr)
	{
		check(characterInstance->GetStatModifier()->GetEffectiveStat().GetEmp() > 0.f);

		float prevEmpPercent = prevValue / characterInstance->GetStatModifier()->GetEffectiveStat().GetEmp();
		float nextEmpPercent = newValue / characterInstance->GetStatModifier()->GetEffectiveStat().GetEmp();
		
		GaugeTweener->Play(prevEmpPercent, nextEmpPercent, 0.2f);
	}
	else if (factorType == EKMStatFactorType::EmpPhaseCurr)
	{
		SetEmpPhaseCurr(static_cast<int32>(newValue + 0.0001f));
	}
}