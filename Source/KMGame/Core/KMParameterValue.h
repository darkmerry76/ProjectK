#pragma once

#include "CoreMinimal.h"
#include "KMParameterLayerSystem.h"
#include "StructUtils/InstancedStruct.h"
#include "KMParameterValue.generated.h"

USTRUCT()
struct FKMParameterValueBase
{
	GENERATED_BODY()

	virtual ~FKMParameterValueBase() {}

	virtual void Add(const FKMParameterValueBase* other) { }
	virtual void Multiply(const FKMParameterValueBase* other) { }
	virtual void Set(const FKMParameterValueBase* other) { }
};

USTRUCT(BlueprintType)
struct KMGAME_API FKMParameterBase
{
	GENERATED_USTRUCT_BODY()

	virtual ~FKMParameterBase() {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FName ParamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	EKMParameterBlendMode BlendMode = EKMParameterBlendMode::Override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	int32 Priority = 0;

protected:
	mutable float CachedMinTime = 0.f;
	mutable float CachedMaxTime = 0.f;

	bool bIsEvaluate = false;
	
protected:
	float MapAlphaToCurveTime(const class UCurveBase* curveBase, float alpha) const;

public:
	virtual void Evaluate(float alpha, TInstancedStruct<FKMParameterValueBase>& outValue) const { };
};

USTRUCT()
struct KMGAME_API FKMParameterScalarValue : public FKMParameterValueBase
{
	GENERATED_USTRUCT_BODY()
	
	float Value = 1.f;

	virtual void Add(const FKMParameterValueBase* other) override
	{
		Value += static_cast<const FKMParameterScalarValue*>(other)->Value;
	}

	virtual void Multiply(const FKMParameterValueBase* other) override
	{
		Value *= static_cast<const FKMParameterScalarValue*>(other)->Value;
	}

	virtual void Set(const FKMParameterValueBase* other) override
	{
		Value = static_cast<const FKMParameterScalarValue*>(other)->Value;
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct KMGAME_API FKMParameterScalar : public FKMParameterBase
{
	GENERATED_USTRUCT_BODY()

	float GetScalarValue(float alpha) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta=(DisplayAfter="Priority"))
	float Value = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta=(DisplayAfter="Value"))
	TObjectPtr<class UCurveFloat> ScalarCurve;
	
	virtual void Evaluate(float alpha, TInstancedStruct<FKMParameterValueBase>& outValue) const override;
};

USTRUCT(BlueprintType, Blueprintable)
struct KMGAME_API FKMParameterSimpleScalar
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float Value = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	TObjectPtr<class UCurveFloat> ScalarCurve;
	
	float Evaluate(float alpha) const;
};

USTRUCT()
struct FKMParameterVectorValue : public FKMParameterValueBase
{
	GENERATED_BODY()

	FVector Value = FVector::One();

	virtual void Add(const FKMParameterValueBase* other) override
	{
		check(FKMParameterVectorValue::StaticStruct() == other->StaticStruct())
		
		Value += static_cast<const FKMParameterVectorValue*>(other)->Value;
	}

	virtual void Multiply(const FKMParameterValueBase* other) override
	{
		check(FKMParameterScalarValue::StaticStruct() == other->StaticStruct())
		
		Value *= static_cast<const FKMParameterVectorValue*>(other)->Value;
	}

	virtual void Set(const FKMParameterValueBase* other) override
	{
		check(FKMParameterScalarValue::StaticStruct() == other->StaticStruct())
		
		Value = static_cast<const FKMParameterVectorValue*>(other)->Value;
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct KMGAME_API FKMParameterVector : public FKMParameterBase
{
	GENERATED_USTRUCT_BODY()

	FVector GetVectorValue(float alpha) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta=(DisplayAfter="Priority"))
	FVector Value = FVector::One();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta=(DisplayAfter="Value"))
	TObjectPtr<class UCurveVector> VectorCurve;

	virtual void Evaluate(float alpha, TInstancedStruct<FKMParameterValueBase>& outValue) const override;
};

USTRUCT()
struct FKMParameterLinearColorValue : public FKMParameterValueBase
{
	GENERATED_BODY()

	FLinearColor Value = FLinearColor::White;

	virtual void Add(const FKMParameterValueBase* other) override
	{
		check(FKMParameterLinearColorValue::StaticStruct() == other->StaticStruct())
		
		Value += static_cast<const FKMParameterLinearColorValue*>(other)->Value;
	}

	virtual void Multiply(const FKMParameterValueBase* other) override
	{
		check(FKMParameterLinearColorValue::StaticStruct() == other->StaticStruct())
		
		Value *= static_cast<const FKMParameterLinearColorValue*>(other)->Value;
	}

	virtual void Set(const FKMParameterValueBase* other) override
	{
		check(FKMParameterLinearColorValue::StaticStruct() == other->StaticStruct())
		
		Value = static_cast<const FKMParameterLinearColorValue*>(other)->Value;
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct KMGAME_API FKMParameterLinearColor : public FKMParameterBase
{
	GENERATED_USTRUCT_BODY()
	
	FLinearColor GetLinearColorValue(float alpha) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta=(DisplayAfter="Priority"))
	FLinearColor Value = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta=(DisplayAfter="Value"))
	TObjectPtr<class UCurveLinearColor> LinearColorCurve;

	virtual void Evaluate(float alpha, TInstancedStruct<FKMParameterValueBase>& outValue) const override;
};