#include "EMTweener.h"

void UEMTweener::FinishDestroy()
{
	StopAnimation();
	TweenDelegates.Empty();
	TweenDynamicDelegates.Empty();
	Super::FinishDestroy();
}
	
void UEMTweener::OnTweenUpdate(eTickerEventType eventType, float deltaTime, float elapsedTime, float duration)
{
	if (!IsValid(GetOuter()))
	{
		StopAnimation();
		return;
	}
	
	for (auto& delegateItr : TweenDelegates)
	{
		delegateItr.ExecuteIfBound(StartValue, NextValue, elapsedTime, duration);
	}

	for (auto& delegateItr : TweenDynamicDelegates)
	{
		delegateItr.ExecuteIfBound(StartValue, NextValue, elapsedTime, duration);
	}
}

void UEMTweener::StopAnimation()
{
	if (TickerHandle.IsValid())
	{
		if (UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this))
		{
			tickerSubsystem->RemoveTicker(TickerHandle);
		}
	}
}

void UEMTweener::AddTweenDynamicDelegate(const FEMTweenDynamicDelegate& newDynamicDelegate)
{
	TweenDynamicDelegates.Emplace(newDynamicDelegate);
}

void UEMTweener::AddTweenDelegate(const FEMTweenDelegate& newDelegate)
{
	TweenDelegates.Emplace(newDelegate);		
}

void UEMTweener::Play(float startValue, float nextValue, float duration) 
{
	StartValue = startValue;
	NextValue = nextValue;
	
	UEMTickerSubsystem* tickerSubsystem = UEMTickerSubsystem::GetTickerSubsystem(this);
	tickerSubsystem->RemoveTicker(TickerHandle);

	FBTMTickerDelegate newTickerDelegate;
	newTickerDelegate.BindUObject(this, &ThisClass::OnTweenUpdate);
	TickerHandle = tickerSubsystem->AddTicker(newTickerDelegate, duration);
};