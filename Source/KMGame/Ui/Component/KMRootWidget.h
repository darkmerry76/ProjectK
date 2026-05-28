#pragma once

#include "CoreMinimal.h"
#include "KMUserWidget.h"
#include "KMRootWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMRootWidget : public UKMUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> RootPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UKMNarrativeWidget> NarrativeWidget;
	
public:
	UKMRootWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;
};
