#include "EMTickerSubsystem.h"
#include "Kismet\GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMTickerSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UEMTickerSubsystem* UEMTickerSubsystem::GetTickerSubsystem(UObject* WorldContextObject)
{
	UGameInstance* GameInstance = Cast<UGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (false == IsValid(GameInstance))
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UEMTickerSubsystem>();
}

FEMTickerHandle UEMTickerSubsystem::AddTicker(UObject* WorldContextObject, FBTMTickerDynamicDelegate EventDelegate, double Duration, double StartEplipseTime)
{
	UEMTickerSubsystem* TickerSubsystem = GetTickerSubsystem(WorldContextObject);
	check(IsValid(TickerSubsystem));

	return TickerSubsystem->AddTicker(EventDelegate, Duration, StartEplipseTime);
}

FEMTickerHandle UEMTickerSubsystem::AddTicker(FBTMTickerDelegate EventDelegate, double Duration, double StartEplipseTime)
{
	if (false == EventDelegate.IsBound())
	{
		return FEMTickerHandle();
	}

	double WorldSeconds = GetWorld()->GetTimeSeconds();

	FEMTickerHandle newTickerHandle;
	newTickerHandle.Data->StartSeconds = WorldSeconds - StartEplipseTime;
	newTickerHandle.Data->Duration = Duration;
	newTickerHandle.Data->Event = EventDelegate;
	newTickerHandle.Data->Event.ExecuteIfBound(eTickerEventType::CREATED, GetWorld()->DeltaTimeSeconds, newTickerHandle.Data->GetElipsedSeconds(WorldSeconds), Duration);

	Tickers.Emplace(newTickerHandle);
	TickersMap.Emplace(newTickerHandle.Data.Get()); 

	return newTickerHandle;	
}

FEMTickerHandle UEMTickerSubsystem::AddTicker(FBTMTickerDynamicDelegate EventDelegate, double Duration, double StartEplipseTime)
{
	if (false == EventDelegate.IsBound())
	{
		return FEMTickerHandle();
	}

	double WorldSeconds = GetWorld()->GetTimeSeconds();

	FEMTickerHandle newTickerHandle;
	newTickerHandle.Data->StartSeconds = WorldSeconds - StartEplipseTime;
	newTickerHandle.Data->Duration = Duration;
	newTickerHandle.Data->EventDynamic = EventDelegate;
	newTickerHandle.Data->EventDynamic.ExecuteIfBound(eTickerEventType::CREATED, GetWorld()->DeltaTimeSeconds, newTickerHandle.Data->GetElipsedSeconds(WorldSeconds), Duration);

	Tickers.Emplace(newTickerHandle);
	TickersMap.Emplace(newTickerHandle.Data.Get()); 

	return newTickerHandle;
}

bool UEMTickerSubsystem::IsTickerValid(FEMTickerHandle Handle, UObject* WorldContextObject)
{
	UEMTickerSubsystem* TickerSubsystem = GetTickerSubsystem(WorldContextObject);
	check(IsValid(TickerSubsystem));

	return TickerSubsystem->IsTickerValid(Handle);
}

bool UEMTickerSubsystem::IsTickerValid(FEMTickerHandle Handle)
{
	if (false == Handle.IsValid())
	{
		return false;
	}
	return TickersMap.Contains(Handle.Data.Get());
}

double UEMTickerSubsystem::GetTickerEplispedSeconds(FEMTickerHandle Handle, UObject* WorldContextObject)
{
	UEMTickerSubsystem* TickerSubsystem = GetTickerSubsystem(WorldContextObject);
	check(IsValid(TickerSubsystem));

	return TickerSubsystem->GetTickerEplispedSeconds(Handle);
}

double UEMTickerSubsystem::GetTickerEplispedSeconds(FEMTickerHandle Handle)
{
	if (false == IsTickerValid(Handle))
	{
		return 0.f;
	}
	return Handle.Data->GetElipsedSeconds(GetWorld()->GetTimeSeconds());
}

double UEMTickerSubsystem::GetTickerDuration(FEMTickerHandle Handle, UObject* WorldContextObject)
{
	UEMTickerSubsystem* TickerSubsystem = GetTickerSubsystem(WorldContextObject);
	check(IsValid(TickerSubsystem));

	return TickerSubsystem->GetTickerDuration(Handle);
}

double UEMTickerSubsystem::GetTickerDuration(FEMTickerHandle Handle)
{
	if (false == IsTickerValid(Handle))
	{
		return 0.f;
	}
	return Handle.Data->Duration;
}

void UEMTickerSubsystem::RemoveTicker(FEMTickerHandle Handle, UObject* WorldContextObject)
{
	UEMTickerSubsystem* TickerSubsystem = GetTickerSubsystem(WorldContextObject);
	check(IsValid(TickerSubsystem));

	TickerSubsystem->RemoveTicker(Handle);
}

void UEMTickerSubsystem::RemoveTickerAt(int32 TickerIndex)
{
	check(TickerIndex < Tickers.Num());
	TickersMap.Remove(Tickers[TickerIndex].Data.Get());
	Tickers.RemoveAt(TickerIndex);
}

void UEMTickerSubsystem::RemoveTicker(FEMTickerHandle Handle)
{
	if (false == Handle.IsValid())
	{
		return;
	}
	for (int32 TickerIndex = 0; TickerIndex < Tickers.Num(); ++TickerIndex)
	{
		if (Tickers[TickerIndex].Data.Get() == Handle.Data.Get())
		{
			float WorldSeconds = GetWorld()->GetTimeSeconds();

			Tickers[TickerIndex].Data->Event.ExecuteIfBound(eTickerEventType::REMOVED, GetWorld()->DeltaTimeSeconds,
				Handle.Data->GetElipsedSeconds(WorldSeconds), Tickers[TickerIndex].Data->Duration);

			Tickers[TickerIndex].Data->EventDynamic.ExecuteIfBound(eTickerEventType::REMOVED, GetWorld()->DeltaTimeSeconds,
		Handle.Data->GetElipsedSeconds(WorldSeconds), Tickers[TickerIndex].Data->Duration);
			
			RemoveTickerAt(TickerIndex);
			return;
		}
	}
}

void UEMTickerSubsystem::RemoveAllTicker(UObject* WorldContextObject)
{
	UEMTickerSubsystem* TickerSubsystem = GetTickerSubsystem(WorldContextObject);
	check(IsValid(TickerSubsystem));

	TickerSubsystem->RemoveAllTicker();
}

void UEMTickerSubsystem::RemoveAllTicker()
{
	Tickers.Empty();
}

void UEMTickerSubsystem::Tick(float DeltaTime)
{
	double WorldSeconds = GetWorld()->GetTimeSeconds();

	for(int32 TickerIndex = 0; TickerIndex < Tickers.Num(); )
	{
		if(false == Tickers[TickerIndex].Data.IsValid())
		{
			RemoveTickerAt(TickerIndex);
			continue;
		}
		double EplipsedSeconds = Tickers[TickerIndex].Data->GetElipsedSeconds(WorldSeconds);
		if (Tickers[TickerIndex].Data->GetElipsedSecondsAbs(WorldSeconds) >= FMath::Abs(Tickers[TickerIndex].Data->Duration))
		{
			Tickers[TickerIndex].Data->Event.ExecuteIfBound(eTickerEventType::UPDATED, DeltaTime, EplipsedSeconds, Tickers[TickerIndex].Data->Duration);
			Tickers[TickerIndex].Data->EventDynamic.ExecuteIfBound(eTickerEventType::REMOVED, DeltaTime, EplipsedSeconds, Tickers[TickerIndex].Data->Duration);
			RemoveTickerAt(TickerIndex);
			continue;
		}
		else
		{
			Tickers[TickerIndex].Data->Event.ExecuteIfBound(eTickerEventType::UPDATED, DeltaTime, EplipsedSeconds, Tickers[TickerIndex].Data->Duration);
			Tickers[TickerIndex].Data->EventDynamic.ExecuteIfBound(eTickerEventType::UPDATED, DeltaTime, EplipsedSeconds, Tickers[TickerIndex].Data->Duration);
		}
		++TickerIndex;
	}
}

TStatId UEMTickerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBTMTickerSubsystem, STATGROUP_Tickables);
}

bool UEMTickerSubsystem::IsTickable() const
{
	return (HasAnyFlags(RF_ClassDefaultObject) == false);
}

ETickableTickType UEMTickerSubsystem::GetTickableTickType() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Always;
}
