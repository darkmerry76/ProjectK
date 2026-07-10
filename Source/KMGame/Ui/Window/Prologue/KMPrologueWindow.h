#pragma once

#include "CoreMinimal.h"
#include "System/EMTickerSubsystem.h"
#include "UI/Component/KMUserWidget.h"
#include "KMPrologueWindow.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMPrologueWindowWidget : public UKMUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<class UEMBorder> Border;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<class UEMScrollBox> ScrollBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<class UEMRichTextBlock> RichTextBlock;
	
	const struct FKMTable_Narrative_PrologueRow* PrologueTableRow = nullptr;

	FEMTickerHandle ScrollTickerHandle;
	
public:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FName PrologueTableId = NAME_None;
	
public:
	void FadeOut();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnScrollAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration);
	void OnFadeinAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration);
	void OnFadeoutAnimation(eTickerEventType eventType, float deltaTime, float eplipseTime, float duration);
};