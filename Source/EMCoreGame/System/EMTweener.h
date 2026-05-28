#pragma once

#include "CoreMinimal.h"
#include "Core/EMDefine.h"
#include "EMTickerSubsystem.h"
#include "EMTweener.generated.h"

UCLASS()
class EMCOREGAME_API UEMTweener : public UObject
{
	GENERATED_BODY()

protected:
	float StartValue = 1.f;
	float NextValue = 1.f;

private:
	FEMTickerHandle TickerHandle;
	TArray<FEMTweenDelegate> TweenDelegates;
	TArray<FEMTweenDynamicDelegate> TweenDynamicDelegates;
	
protected:
	virtual void FinishDestroy() override;

private:
	UFUNCTION()
	void OnTweenUpdate(eTickerEventType eventType, float deltaTime, float elapsedTime, float duration);

public:
	UFUNCTION(BlueprintCallable)
	void StopAnimation();

	UFUNCTION(BlueprintCallable)
	void AddTweenDynamicDelegate(const FEMTweenDynamicDelegate& newDynamicDelegate);

	void AddTweenDelegate(const FEMTweenDelegate& newDelegate);

	UFUNCTION(BlueprintCallable)
	void Play(float startValue, float nextValue, float duration); 
};