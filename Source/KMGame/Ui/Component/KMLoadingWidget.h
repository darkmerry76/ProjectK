#pragma once

#include "CoreMinimal.h"
#include "KMUserWidget.h"
#include "KMLoadingWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMLoadingWidget : public UKMUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEMImage> Image;
	
public:
	UKMLoadingWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;
};
