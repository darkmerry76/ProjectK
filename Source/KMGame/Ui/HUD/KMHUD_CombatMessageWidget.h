#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "Core/KMDefine.h"
#include "KMHUD_CombatMessageWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_CombatMessageWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMScrollBox* MessageScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float ScrollSpeed = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TSubclassOf<class UKMHUD_CombatMessageItemWidget> MessageItemWidgetClass;

public:
	UKMHUD_CombatMessageWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Refresh();

	void OnCommbatMessage(const class UKMGameObjectInstance* gameObjectInstance, EKMCommbatMessageType messageType, const FString& newMessage);
	void AddMessage(const FText& messageText, EKMCommbatMessageType messageType, const FLinearColor& fontColor, float fontSize, float duration);

	virtual void NativeTick(const FGeometry& myGeometry, float deltaTime) override;
};
