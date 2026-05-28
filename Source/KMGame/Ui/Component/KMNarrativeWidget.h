#pragma once

#include "CoreMinimal.h"
#include "KMUserWidget.h"
#include "KMNarrativeWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMNarrativeWidget : public UKMUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEMTextBlock> MessageText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEMTextBlock> MessageText1;

	int32 MessageRow = 0;
	
public:
	UKMNarrativeWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;

	void SetMessageText(const FText& text, const FLinearColor& color, bool nessageClear = false);
	void ClearText();
};
