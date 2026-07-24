#pragma once

#include "CoreMinimal.h"
#include "Notify/EMAnimNotifyMA.h"
#include "KMAnimNotify_Stiff.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotify_Stiff
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Stiff")
class KMGAME_API UKMAnimNotify_Stiff : public UEMAnimNotifyMA
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	float Duration = 0.2f;
	
public:
	UKMAnimNotify_Stiff();

protected:
	virtual void Notify(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;
	
	void DoStiff(USkeletalMeshComponent* meshComp, float stiffDuration);
	
protected:
	virtual FString GetNotifyName_Implementation() const override;
};
