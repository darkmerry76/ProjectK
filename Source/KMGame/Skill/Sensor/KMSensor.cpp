#include "KMSensor.h"

UKMSensor::UKMSensor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMSensor::Init()
{
	UWorld* world = GetWorld();
	check(IsValid(world));
	
	world->GetTimerManager().SetTimer(TickHandle, FTimerDelegate::CreateUObject(this, &UKMSensor::DoSearch), IntervalSeconds, true);
}

void UKMSensor::DoSearch()
{
}

void UKMSensor::CleatTimer()
{
	if (UWorld* world = GetWorld())
	{
		world->GetTimerManager().ClearTimer(TickHandle);
	}
}

void UKMSensor::BeginDestroy()
{
	Super::BeginDestroy();

	CleatTimer();
}

void UKMSensor::SetCenterTransform(const FTransform& newCenterTransform)
{
	CenterTransform = newCenterTransform;
}

const FTransform& UKMSensor::GetCenterTransform() const
{
	return CenterTransform;
}