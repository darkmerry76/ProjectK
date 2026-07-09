#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "UI/Component/KMUserWidget.h"
#include "KMPopupMenuWidget.generated.h"

DECLARE_DELEGATE_OneParam(FKMPopupSelectDelegate, EKMPopupButtonType buttonType);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMPopupMenuWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget,AllowPrivateAccess=true))
	TObjectPtr<class UEMWidgetSwitcher> ButtonTypeSwitcher;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<class UEMTextBlock> TitleTextBlock;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<class UEMRichTextBlock> MessageRichTextBlock;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	EKMPopupType PopupType = EKMPopupType::OkCancel;
	
public:
	FKMPopupSelectDelegate PopupSelectDelegate; 

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnSelectButton(const EKMPopupButtonType buttonType);

	UFUNCTION()
	void OnYes();

	UFUNCTION()
	void OnNo();

	UFUNCTION()
	void OnOk();

	UFUNCTION()
	void OnCancel();

public:
	UFUNCTION(BlueprintCallable)
	void SetPopupType(EKMPopupType newPopupType);

	UFUNCTION(BlueprintPure)
	EKMPopupType GetPopupType() const;

	UFUNCTION(BlueprintCallable)
	void SetTitleText(const FString newMessageText);

	UFUNCTION(BlueprintCallable)
	void SetMessageText(const FString newMessageText);
};