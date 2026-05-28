#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EMTickerSubsystem.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// eTickerEventType
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class eTickerEventType : uint8
{
	CREATED,
	UPDATED,
	REMOVED,
};

DECLARE_DELEGATE_FourParams(FBTMTickerDelegate, eTickerEventType eventType, float deltaTime, float eplipseTime, float duration);
DECLARE_DYNAMIC_DELEGATE_FourParams(FBTMTickerDynamicDelegate, eTickerEventType, eventType, float, deltaTime, float, eplipseTime, float, duration);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMTickerHandle
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FEMTickerHandle
{
	GENERATED_BODY()

public:
	struct FTickerData
	{
		double StartSeconds = 0.f;
		double Duration = 0.f;

		float GetAbsDuration() const
		{
			return FMath::Abs(Duration);
		}
		float GetElipsedSecondsAbs(double WorldSeconds) const
		{
			double EplipsedSeconds = FMath::Clamp(WorldSeconds - StartSeconds, 0.f, GetAbsDuration());
			return EplipsedSeconds;
		}

		float GetElipsedSeconds(double WorldSeconds) const
		{
			double EplipsedSeconds = FMath::Clamp(WorldSeconds - StartSeconds, 0.f, GetAbsDuration());
			return Duration < 0.f ? GetAbsDuration() - EplipsedSeconds : EplipsedSeconds;
		}
		bool IsTickerValid() const
		{
			return Event.IsBound();
		}

		FBTMTickerDelegate Event;
		FBTMTickerDynamicDelegate EventDynamic;
	};

public:
	FEMTickerHandle()
	{
		Data = MakeShareable(new FTickerData());
	}
	bool IsValid() const
	{
		if (false == Data.IsValid())
		{
			return false;
		}
		return Data->IsTickerValid();
	}
	TSharedPtr<FTickerData> Data;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMTickerSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, Abstract)
class EMCOREGAME_API UEMTickerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static UEMTickerSubsystem* GetTickerSubsystem(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static FEMTickerHandle AddTicker(UObject* WorldContextObject, FBTMTickerDynamicDelegate EventDelegate, double Duration = 1.f, double StartElapsedTime = 0.f);

	FEMTickerHandle AddTicker(FBTMTickerDelegate EventDelegate, double Duration = 1.f, double StartEplipseTime = 0.f);
	FEMTickerHandle AddTicker(FBTMTickerDynamicDelegate EventDelegate, double Duration = 1.f, double StartEplipseTime = 0.f);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static bool IsTickerValid(FEMTickerHandle Handle, UObject* WorldContextObject);
	bool IsTickerValid(FEMTickerHandle Handle);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void RemoveTicker(FEMTickerHandle Handle, UObject* WorldContextObject);
	void RemoveTicker(FEMTickerHandle Handle);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void RemoveAllTicker(UObject* WorldContextObject);
	void RemoveAllTicker();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static double GetTickerEplispedSeconds(FEMTickerHandle Handle, UObject* WorldContextObject);
	double GetTickerEplispedSeconds(FEMTickerHandle Handle);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static double GetTickerDuration(FEMTickerHandle Handle, UObject* WorldContextObject);
	double GetTickerDuration(FEMTickerHandle Handle);

	void RemoveTickerAt(int32 TickerIndex);

protected:
	// FTickableGameObject begin
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	// FTickableGameObject end

protected:
	TArray<FEMTickerHandle> Tickers;
	TSet<FEMTickerHandle::FTickerData*> TickersMap;
};