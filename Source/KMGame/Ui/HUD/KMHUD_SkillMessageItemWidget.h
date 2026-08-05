#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "UI/Component/KMUserWidget.h"
#include "KMHUD_SkillMessageItemWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHUD_SkillMessageItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMTextBlock* MessageTextBlock;
	
public:
	UKMHUD_SkillMessageItemWidget(const FObjectInitializer& objectInitializer);
	
	void SetMessage(const FText& messageText);
	void SetFontColor(const FLinearColor& color);
	void SetFontSize(float newFontSize);

	void Refresh();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
