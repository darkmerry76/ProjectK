#include "KMParameterValue.h"
#include "Curves/CurveLinearColor.h"
#include "Curves/CurveVector.h"

float FKMParameterBase::MapAlphaToCurveTime(const UCurveBase* curveBase, float alpha) const
{
	if (!IsValid(curveBase))
	{
		return alpha;
	}
	if (FMath::IsNearlyZero(CachedMinTime + CachedMaxTime))
	{
		curveBase->GetTimeRange(CachedMinTime, CachedMaxTime);
	}

	return FMath::Lerp(CachedMinTime, CachedMaxTime, FMath::Clamp(alpha, 0.f,1.f));
}

float FKMParameterScalar::GetScalarValue(float alpha) const
{
	if (IsValid(ScalarCurve))
	{
		return Value * ScalarCurve->GetFloatValue(MapAlphaToCurveTime(ScalarCurve, alpha));
	}
	return Value;
}

void FKMParameterScalar::Evaluate(float alpha, TInstancedStruct<FKMParameterValueBase>& outValue) const
{
	outValue.InitializeAs<FKMParameterScalarValue>();

	float v = GetScalarValue(alpha);
	outValue.GetMutable<FKMParameterScalarValue>().Value = v;
}

FVector FKMParameterVector::GetVectorValue(float alpha) const
{
	if (IsValid(VectorCurve))
	{
		return Value * VectorCurve->GetVectorValue(MapAlphaToCurveTime(VectorCurve, alpha));
	}
	return Value;
}

void FKMParameterVector::Evaluate(float alpha, TInstancedStruct<FKMParameterValueBase>& outValue) const
{
	outValue.InitializeAs<FKMParameterVectorValue>();

	FVector v = GetVectorValue(alpha);
	outValue.GetMutable<FKMParameterVectorValue>().Value = v;
}

FLinearColor FKMParameterLinearColor::GetLinearColorValue(float alpha) const
{
	if (IsValid(LinearColorCurve))
	{
		return Value * LinearColorCurve->GetLinearColorValue(MapAlphaToCurveTime(LinearColorCurve, alpha));
	}
	return Value;
}

void FKMParameterLinearColor::Evaluate(float alpha, TInstancedStruct<FKMParameterValueBase>& outValue) const
{
	outValue.InitializeAs<FKMParameterLinearColorValue>();

	FLinearColor v = GetLinearColorValue(alpha);
	outValue.GetMutable<FKMParameterLinearColorValue>().Value = v;
}
