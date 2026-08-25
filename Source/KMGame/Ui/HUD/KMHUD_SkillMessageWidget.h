#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "Core/KMDefine.h"
#include "KMHUD_SkillMessageWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_SkillMessageWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMComboBoxString* CharacterCombo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMScrollBox* MessageScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float ScrollSpeed = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TSubclassOf<class UKMHUD_SkillMessageItemWidget> MessageItemWidgetClass;

public:
	UKMHUD_SkillMessageWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();

	void OnSkillMessage(const class UKMGameObjectInstance* gameObjectInstance, TSharedPtr<class FKMAbilityInstanceBase> abilityInstance, const FString& prefixMessage);
	void AddMessage(const FText& messageText, const FLinearColor& fontColor, float fontSize);

	virtual void NativeTick(const FGeometry& myGeometry, float deltaTime) override;

	void OnAddCharacter(class UKMGameObjectInstance* gameObjectInstance);
};
