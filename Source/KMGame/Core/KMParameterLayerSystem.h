#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "KMParameterLayerSystem.generated.h"

UENUM()
enum class EKMParameterBlendMode : uint8
{
	Multiply,
	Add,
	Override
};

USTRUCT()
struct FKMParameterLayer
{
	GENERATED_BODY()

	UPROPERTY()
	FName LayerName;
	
	UPROPERTY()
	FName ParamName;

	UPROPERTY()
	TInstancedStruct<struct FKMParameterBase> Param;

	UPROPERTY()
	mutable float Alpha = 0.f;

	UPROPERTY()
	EKMParameterBlendMode BlendMode = EKMParameterBlendMode::Multiply;

	UPROPERTY()
	int32 Priority = 0;
};

DECLARE_DELEGATE_TwoParams(FKMApplyScalarDelegate, const FName&, float);
struct FKMParameterLayerSystem
{
public:
	TArray<FKMParameterLayer> ActiveLayers;
	TMap<FName, TInstancedStruct<struct FKMParameterValueBase>> BaseValues;

	float Alpha = 0.f;

public:
	void SetBaseValue(const FName& paramName, const TInstancedStruct<FKMParameterValueBase>& baseValue);
	void RemoveBaseValue(const FName& paramName);
	const TInstancedStruct<struct FKMParameterValueBase>* GetBaseValue(const FName& paramName) const;

	void AddAlpha(const FName& layerName, float newAlpha);
	void SetAlpha(const FName& layerName, float newAlpha);
	float GetAlpha(const FName& layerName) const;
	
	FKMParameterLayer& AddLayer(
		const FName& layerName,
		const FName& paramName,
		const TInstancedStruct<struct FKMParameterBase>& param,
		EKMParameterBlendMode blendMode = EKMParameterBlendMode::Multiply,
		int32 priority = 0);

	const FKMParameterLayer* FindLayersByName(const FName& layerName) const;
	
	void RemoveLayersByName(const FName& layerName);
	
	void Evaluate(TMap<FName, TInstancedStruct<struct FKMParameterValueBase>>& outResults) const;
};

class FKMSkeletalMeshComponentParameterLayer
{
protected:
	TMap<int32, FKMParameterLayerSystem> MaterialParameterLayer;
	TWeakObjectPtr<class USkeletalMeshComponent> SkeletalMeshComponent;

public:
	FKMSkeletalMeshComponentParameterLayer(class USkeletalMeshComponent* skeletalMeshComponent);
	void InitializeComponent();
	void InitBaseFromMaterial(const class UMaterialInterface* material, FKMParameterLayerSystem& outSystem);
	
	void AddMaterialParameterLayer(const FName& layerName, const FName& parameterName,
		const TInstancedStruct<struct FKMParameterBase>& parameter,
		EKMParameterBlendMode blendMode = EKMParameterBlendMode::Multiply,
		int32 priority = 0);

	void AddMaterialParameterAlpha(const FName& layerName, float alpha);
	void RemoveMaterialParameterLayer(const FName& layerName);
	void SetMaterialParameterAlpha(const FName& layerName, float alpha);

	void ApplyMaterialParameter();
	void SetMaterial(int32 ElementIndex, UMaterialInterface* Material);
};

UCLASS()
class UKMParameterLayerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="worldContextObject"))
	static UKMParameterLayerSubsystem* GetParameterLayerSubsystem(const UObject* worldContextObject);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	TSharedPtr<FKMSkeletalMeshComponentParameterLayer> GetSkeletalMeshComponentLayer(class USkeletalMeshComponent* mesh);

	UFUNCTION()
	void OnComponentRemoved(UActorComponent* Component);

	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
private:
	TMap<TWeakObjectPtr<class USkeletalMeshComponent>, TSharedPtr<FKMSkeletalMeshComponentParameterLayer>> Systems;
};