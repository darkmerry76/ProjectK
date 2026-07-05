#pragma once

#include "CoreMinimal.h"
#include "UI/Component/KMUserWidget.h"
#include "KMCharacterSelectWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMCharacterSelectWidget : public UKMUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMHorizontalBox> CharacterHorizontalBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	TObjectPtr<class UEMButton> EnterButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TSubclassOf<class UKMCharacterSelectItemWidget> CharacterItemClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UKMCharacterSelectItemWidget> PrevSelectedItem;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UKMCharacterSelectItemWidget> PrevHoveredItem;

	FName DefaultHeroId = TEXT("H_RyuX");
	
public:
	UKMCharacterSelectWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnWorldLoadingComplete();

	void Refresh();

	UFUNCTION(BlueprintNativeEvent)
	void OnClicked(class UKMCharacterSelectItemWidget* charactrerSelectItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnHovered(class UKMCharacterSelectItemWidget* charactrerSelectItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnUnhovered(class UKMCharacterSelectItemWidget* charactrerSelectItem);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnterPressed();

protected:
	void SelectCharacter(class UKMCharacterSelectItemWidget* charactrerSelectItem);
};
