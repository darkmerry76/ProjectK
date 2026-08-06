#include "KMHUD_SkillMessageWidget.h"
#include "GameObject/KMCharacterInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "Tables/Generated/KMTable_Skill.h"
#include "Tables/Generated/KMTable_SkillEffect.h"
#include "UI/Component/EMComboBox.h"
#include "Ui/HUD/KMHUD_SkillMessageItemWidget.h"
#include "UI/Component/EMScrollBox.h"

UKMHUD_SkillMessageWidget::UKMHUD_SkillMessageWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHUD_SkillMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	if(IsValid(gameObjectSubsystem))
	{
		CharacterCombo->ClearOptions();
		CharacterCombo->AddOption(TEXT("All"));
		CharacterCombo->SetSelectedOption(TEXT("All"));
		
		gameObjectSubsystem->GetSpawnGameObjectInstanceDelegate().AddUObject(this, &ThisClass::OnAddCharacter);
		for (auto objectItr = gameObjectSubsystem->GetGameObjectMap().CreateConstIterator(); objectItr; ++objectItr)
		{
			OnAddCharacter(Cast<UKMGameObjectInstance>(objectItr.Value()));
		}

		UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
		if(IsValid(authCharacterInstance))
		{
			authCharacterInstance->GetSkillMessageDelegate().AddUObject(this, &ThisClass::OnSkillMessage);
		}
	}
	
	Refresh();
}

void UKMHUD_SkillMessageWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	if(IsValid(gameObjectSubsystem))
	{
		UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
		if(IsValid(authCharacterInstance))
		{
			authCharacterInstance->GetCombatMessageDelegate().RemoveAll(this);
		}
	}
}

void UKMHUD_SkillMessageWidget::OnAddCharacter(UKMGameObjectInstance* gameObjectInstance)
{
	UKMCharacterInstance* characterInstance = Cast<UKMCharacterInstance>(gameObjectInstance);
	if (!IsValid(characterInstance))
	{
		return;
	}
	CharacterCombo->AddOption(characterInstance->GetName());
}

void UKMHUD_SkillMessageWidget::Refresh()
{
}

void UKMHUD_SkillMessageWidget::AddMessage(const FText& messageText, const FLinearColor& fontColor, float fontSize)
{
	UKMHUD_SkillMessageItemWidget* newMessageItemWidget = Cast<UKMHUD_SkillMessageItemWidget>(CreateWidget<UKMHUD_SkillMessageItemWidget>(GetWorld(), MessageItemWidgetClass));
	check(newMessageItemWidget);

	newMessageItemWidget->SetFontColor(fontColor);
	newMessageItemWidget->SetMessage(messageText);
	newMessageItemWidget->SetFontSize(fontSize);

	const float tolerance = 2.0f;

	const bool bAtEnd = FMath::Abs(MessageScrollBox->GetScrollOffsetOfEnd() - MessageScrollBox->GetScrollOffset()) < tolerance;
	MessageScrollBox->AddChild(newMessageItemWidget);

	if (bAtEnd == true)
	{
		MessageScrollBox->ScrollToEnd();
	}
}

void UKMHUD_SkillMessageWidget::OnSkillMessage(const UKMCharacterInstance* characterInstance, TSharedPtr<FKMAbilityInstanceBase> abilityInstance, const FString& prefixMessage)
{
	check(IsValid(characterInstance));

	if (!abilityInstance.IsValid())
	{
		return;
	}

	if (!CharacterCombo->GetSelectedOption().Equals(TEXT("All")) && !CharacterCombo->GetSelectedOption().Equals(characterInstance->GetName()))
	{
		return;
	}
	
	if (abilityInstance->IsA<FKMSkillInstance>())
	{
		TSharedPtr<FKMSkillInstance> skillInstance = StaticCastSharedPtr<FKMSkillInstance>(abilityInstance);
		AddMessage(FText::FromString(FString::Printf(TEXT("%.3f [%s] %s%s"), GetWorld()->GetTimeSeconds(), *characterInstance->GetName(), *prefixMessage, *skillInstance->SkillKey.TableRecord->Id.ToString())), FLinearColor::White, 14.f);
	}
	else if (abilityInstance->IsA<FKMSkillEffectInstance>())
	{
		TSharedPtr<FKMSkillEffectInstance> skillEffectInstance = StaticCastSharedPtr<FKMSkillEffectInstance>(abilityInstance);
		AddMessage(FText::FromString(FString::Printf(TEXT("%.3f [%s] %s%s"), GetWorld()->GetTimeSeconds(), *characterInstance->GetName(), *prefixMessage, *skillEffectInstance->GetEffectTableRecord()->Id.ToString())), FLinearColor::Red, 14.f);
	}
}

void UKMHUD_SkillMessageWidget::NativeTick(const FGeometry& myGeometry, float deltaTime)
{
	Super::NativeTick(myGeometry, deltaTime);
}