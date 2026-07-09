#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "UI/Component/KMUserWidget.h"
#include "KMPopupMenuItemWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMPopupMenuItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<class UButton> Button;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<class UTextBlock> ButtonTextBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	EKMPopupButtonType ButtonType = EKMPopupButtonType::Ok;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintPure)
	EKMPopupButtonType GetButtonType() const;

	UFUNCTION(BlueprintPure)
	class UButton* GetButton() const;

	UFUNCTION(BlueprintPure)
	class UTextBlock* GetButtonTextBlock() const;
};