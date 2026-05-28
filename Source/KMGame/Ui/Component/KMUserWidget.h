#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KMUserWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UKMUserWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	virtual class UWidgetAnimation* GetAnimationByName(FName AnimationName) const;
};
