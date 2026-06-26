#include "KMCameralayerBase.h"

UKMCameralayerBase::UKMCameralayerBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCameralayerBase::SetAlpha(float newAlpha)
{
	Alpha = newAlpha;
}

float UKMCameralayerBase::GetAlpha() const
{
	return Alpha;
}

void UKMCameralayerBase::Evaluate(float deltaTime, FEMCameraOutput& output)
{
}