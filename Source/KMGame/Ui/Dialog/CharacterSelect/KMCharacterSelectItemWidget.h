#pragma once

#include "CoreMinimal.h"
#include "System/EMTickerSubsystem.h"
#include "UI/Component/KMUserWidget.h"
#include "KMCharacterSelectItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMCharacterSelectItemClickedDelegate, class UKMCharacterSelectItemWidget*, characterSelectItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMCharacterSelectItemHoverDelegate, class UKMCharacterSelectItemWidget*, characterSelectItemWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEMCharacterSelectItemUnhoverDelegate, class UKMCharacterSelectItemWidget*, characterSelectItemWidget);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMCharacterSelectItemWidget : public UKMUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = true))
	class UEMButton* CharacterButton;
	
	FEMTickerHandle TickerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredScale = 1.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float SelectedScale = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float HoveredAnimatinTime = 0.2f;

	float NextRenderScale = 1.f;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FName CharacterId;

public:
	bool bIsSelected = false;
	
	UPROPERTY(BlueprintAssignable)
	FEMCharacterSelectItemClickedDelegate ClickedDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMCharacterSelectItemHoverDelegate HoverDelegate;

	UPROPERTY(BlueprintAssignable)
	FEMCharacterSelectItemUnhoverDelegate UnhoverDelegate;
	
public:
	UKMCharacterSelectItemWidget(const FObjectInitializer& objectInitializer);

	UFUNCTION(BlueprintCallable)
	void HoveredAnimation();

	UFUNCTION(BlueprintCallable)
	void UnhoveredAnimation();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnClicked();

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnUnhovered();

	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;
};
