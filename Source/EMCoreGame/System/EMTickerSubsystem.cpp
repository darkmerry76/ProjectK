#include "EMTickerSubsystem.h"
#include "Kismet\GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMTickerSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UEMTickerSubsystem* UEMTickerSubsystem::GetTickerSubsystem(UObject* worldContextObject)
{
	if (!IsValid(worldContextObject))
	{
		return nullptr;
	}
	UWorld* world = worldContextObject->GetWorld();
	if (!IsValid(world))
	{
		return nullptr;
	}
	return world->GetSubsystem<UEMTickerSubsystem>();
}

void UEMTickerSubsystem::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	FWorldDelegates::OnWorldTickStart.AddUObject(this, &UEMTickerSubsystem::Tick);
}

void UEMTickerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldTickStart.RemoveAll(this);
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

void UEMTickerSubsystem::Tick(UWorld* world, ELevelTick levelTick, float deltaTime)
{
	if (world != GetWorld())
	{
		return;
	}
	
	float worldDeltaSeconds = GetWorld()->GetTimeSeconds();

	for(int32 tickerIndex = 0; tickerIndex < Tickers.Num(); )
	{
		if(false == Tickers[tickerIndex].Data.IsValid())
		{
			RemoveTickerAt(tickerIndex);
			continue;
		}
		double elipsedSeconds = Tickers[tickerIndex].Data->GetElipsedSeconds(worldDeltaSeconds);
		if (Tickers[tickerIndex].Data->GetElipsedSecondsAbs(worldDeltaSeconds) >= FMath::Abs(Tickers[tickerIndex].Data->Duration))
		{
			Tickers[tickerIndex].Data->Event.ExecuteIfBound(eTickerEventType::REMOVED, worldDeltaSeconds, elipsedSeconds, Tickers[tickerIndex].Data->Duration);
			Tickers[tickerIndex].Data->EventDynamic.ExecuteIfBound(eTickerEventType::REMOVED, worldDeltaSeconds, elipsedSeconds, Tickers[tickerIndex].Data->Duration);
			RemoveTickerAt(tickerIndex);
			continue;
		}
		else
		{
			Tickers[tickerIndex].Data->Event.ExecuteIfBound(eTickerEventType::UPDATED, deltaTime, elipsedSeconds, Tickers[tickerIndex].Data->Duration);
			Tickers[tickerIndex].Data->EventDynamic.ExecuteIfBound(eTickerEventType::UPDATED, deltaTime, elipsedSeconds, Tickers[tickerIndex].Data->Duration);
		}
		++tickerIndex;
	}
}
bool UEMTickerSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return true;
}