#pragma once

#include "CoreMinimal.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "EMPawnMoverComponent.generated.h"

UCLASS(Abstract)
class EMCOREGAME_API UEMPawnMoverComponent : public UCharacterMoverComponent
{
	GENERATED_BODY()

protected:
	FVector RootMotionTranslationScale = FVector::One();

public:
	UEMPawnMoverComponent();

	virtual FVector ConsumeInputVector() const;

	UFUNCTION(BlueprintCallable)
	void SetAnimRootMotionTranslationScale(const FVector& newScale);

	UFUNCTION(BlueprintPure)
	const FVector& GetAnimRootMotionTranslationScale() const;
	
protected:
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
	virtual void InitializeComponent() override;
};