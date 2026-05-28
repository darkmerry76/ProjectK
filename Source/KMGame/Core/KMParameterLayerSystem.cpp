#include "KMParameterLayerSystem.h"
#include "KMParameterValue.h"

FORCEINLINE int32 GetBlendOrder(EKMParameterBlendMode mode)
{
	switch (mode)
	{
	case EKMParameterBlendMode::Multiply: return 0;
	case EKMParameterBlendMode::Add:      return 1;
	case EKMParameterBlendMode::Override: return 2;
	}
	return 0;
}

void FKMParameterLayerSystem::SetBaseValue( const FName& paramName, const TInstancedStruct<FKMParameterValueBase>& baseValue)
{
	BaseValues.Add(paramName, baseValue);
}

void FKMParameterLayerSystem::RemoveBaseValue(const FName& paramName)
{
	BaseValues.Remove(paramName);
}
	
const TInstancedStruct<FKMParameterValueBase>* FKMParameterLayerSystem::GetBaseValue(const FName& paramName) const
{
	return BaseValues.Find(paramName);
}

FKMParameterLayer& FKMParameterLayerSystem::AddLayer(
	const FName& layerName,
	const FName& paramName,
	const TInstancedStruct<FKMParameterBase>& param,
	EKMParameterBlendMode blendMode,
	int32 priority)
{
	FKMParameterLayer& layer = ActiveLayers.AddDefaulted_GetRef();

	layer.ParamName = paramName;
	layer.LayerName = layerName;
	layer.Param = param;
	layer.BlendMode = blendMode;
	layer.Priority = priority;
	layer.Alpha = 0.f;
	
	ActiveLayers.Sort([](const FKMParameterLayer& A, const FKMParameterLayer& B)
	{
		int32 orderA = GetBlendOrder(A.BlendMode);
		int32 orderB = GetBlendOrder(B.BlendMode);

		if (orderA == orderB)
		{
			return A.Priority > B.Priority;
		}
		return orderA < orderB;
	});

	return layer;
}

void FKMParameterLayerSystem::RemoveLayersByName(const FName& layerName)
{
	ActiveLayers.RemoveAll(
		[layerName](const FKMParameterLayer& layer)
		{
			return layer.LayerName == layerName;
		});
}

const FKMParameterLayer* FKMParameterLayerSystem::FindLayersByName(const FName& layerName) const
{
	return ActiveLayers.FindByPredicate(
	[layerName](const FKMParameterLayer& layer)
		{
			return layer.LayerName == layerName;
		});
}

void FKMParameterLayerSystem::AddAlpha(const FName& layerName, float newAlpha)
{
	if (const FKMParameterLayer* parameterLayer = FindLayersByName(layerName))
	{
		parameterLayer->Alpha += newAlpha;
	}
}

void FKMParameterLayerSystem::SetAlpha(const FName& layerName, float newAlpha)
{
	if (const FKMParameterLayer* parameterLayer = FindLayersByName(layerName))
	{
		parameterLayer->Alpha = newAlpha;
	}
}

float FKMParameterLayerSystem::GetAlpha(const FName& layerName) const
{
	if (const FKMParameterLayer* parameterLayer = FindLayersByName(layerName))
	{
		return parameterLayer->Alpha;
	}
	return 0.f;
}

void FKMParameterLayerSystem::Evaluate(TMap<FName, TInstancedStruct<FKMParameterValueBase>>& outResults) const
{
	outResults.Empty();

	for (const auto& baseItr : BaseValues)
	{
		outResults.Add(baseItr.Key, baseItr.Value);
	}
	
	for (const FKMParameterLayer& layer : ActiveLayers)
	{
		TInstancedStruct<FKMParameterValueBase> value;
		layer.Param.Get<FKMParameterBase>().Evaluate(layer.Alpha, value);

		auto* result = outResults.Find(layer.ParamName);

		if (!result)
		{
			outResults.Add(layer.ParamName, value);
			continue;
		}

		switch (layer.BlendMode)
		{
		case EKMParameterBlendMode::Multiply:
			result->GetMutable<FKMParameterValueBase>().Multiply(&value.Get<FKMParameterValueBase>());
			break;

		case EKMParameterBlendMode::Add:
			result->GetMutable<FKMParameterValueBase>().Add(&value.Get<FKMParameterValueBase>());
			break;

		case EKMParameterBlendMode::Override:
			*result = value;
			break;
		}
	}
}

FKMSkeletalMeshComponentParameterLayer::FKMSkeletalMeshComponentParameterLayer(USkeletalMeshComponent* skeletalMeshComponent) : SkeletalMeshComponent(skeletalMeshComponent)
{
}

void FKMSkeletalMeshComponentParameterLayer::InitBaseFromMaterial(const UMaterialInterface* material, FKMParameterLayerSystem& outSystem)
{
	TMap<FMaterialParameterInfo, FMaterialParameterMetadata> scalarParams;
	material->GetAllParametersOfType(EMaterialParameterType::Scalar, scalarParams);

	for (auto& scalarItr : scalarParams)
	{
		float value = 0.f;
		material->GetScalarParameterValue(scalarItr.Key, value);

		TInstancedStruct<FKMParameterScalarValue> param;
		param.InitializeAs<FKMParameterScalarValue>();
		param.GetMutable<FKMParameterScalarValue>().Value = value;

		outSystem.SetBaseValue(scalarItr.Key.Name, param);
	}

	TMap<FMaterialParameterInfo, FMaterialParameterMetadata> vectorParams;
	material->GetAllParametersOfType(EMaterialParameterType::Vector, vectorParams);

	for (auto& vectorItr : vectorParams)
	{
		FLinearColor value;
		material->GetVectorParameterValue(vectorItr.Key, value);

		TInstancedStruct<FKMParameterLinearColorValue> param;
		param.InitializeAs<FKMParameterLinearColorValue>();
		param.GetMutable<FKMParameterLinearColorValue>().Value = value;

		outSystem.SetBaseValue(vectorItr.Key.Name, param);
	}
}

void FKMSkeletalMeshComponentParameterLayer::InitializeComponent()
{
	MaterialParameterLayer.Reset();
	
	for (int32 materialIndex = 0; materialIndex < SkeletalMeshComponent->GetNumMaterials(); ++materialIndex)
	{
		UMaterialInterface* material = SkeletalMeshComponent->GetMaterial(materialIndex);
		if (!IsValid(material))
		{
			continue;
		}
		
		FKMParameterLayerSystem newMaterialLayerSystem;
		InitBaseFromMaterial(material, newMaterialLayerSystem);
		
		MaterialParameterLayer.Emplace(materialIndex, MoveTemp(newMaterialLayerSystem));
	}
}

void FKMSkeletalMeshComponentParameterLayer::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	if (ElementIndex < 0)
	{
		return;
	}

	if (!IsValid(Material))
	{
		MaterialParameterLayer.Remove(ElementIndex);
		return;
	}

	FKMParameterLayerSystem newLayerSystem;

	InitBaseFromMaterial(Material, newLayerSystem);
	MaterialParameterLayer.FindOrAdd(ElementIndex) = MoveTemp(newLayerSystem);
}

void FKMSkeletalMeshComponentParameterLayer::AddMaterialParameterLayer(const FName& layerName,
	const FName& parameterName, const TInstancedStruct<struct FKMParameterBase>& parameter, EKMParameterBlendMode blendMode, int32 priority)
{
	for (auto& materialLayer : MaterialParameterLayer)
	{
		materialLayer.Value.AddLayer(layerName, parameterName, parameter, blendMode, priority);
	}
}

void FKMSkeletalMeshComponentParameterLayer::RemoveMaterialParameterLayer(const FName& layerName)
{
	for (auto& materialLayer : MaterialParameterLayer)
	{
		materialLayer.Value.RemoveLayersByName(layerName);
	}
}

void FKMSkeletalMeshComponentParameterLayer::AddMaterialParameterAlpha(const FName& layerName, float alpha)
{
	for (auto& materialLayerItr : MaterialParameterLayer)
	{
		materialLayerItr.Value.AddAlpha(layerName, alpha);
	}
}

void FKMSkeletalMeshComponentParameterLayer::SetMaterialParameterAlpha(const FName& layerName, float alpha)
{
	for (auto& materialLayerItr : MaterialParameterLayer)
	{
		materialLayerItr.Value.SetAlpha(layerName, alpha);
	}
}

void FKMSkeletalMeshComponentParameterLayer::ApplyMaterialParameter()
{
	for (auto& materialLayerItr : MaterialParameterLayer)
	{
		TMap<FName, TInstancedStruct<FKMParameterValueBase>> paramResults;
		materialLayerItr.Value.Evaluate(paramResults);

		UMaterialInstanceDynamic* materialDyanmic = SkeletalMeshComponent->CreateDynamicMaterialInstance(materialLayerItr.Key);
		if (!IsValid(materialDyanmic))
		{
			continue;
		}

		for (auto paramItr : paramResults)
		{
			if (!paramItr.Value.IsValid())
			{
				continue;
			}
			
			if(const FKMParameterScalarValue* parameterScalar = paramItr.Value.GetPtr<FKMParameterScalarValue>())
			{
				materialDyanmic->SetScalarParameterValue(paramItr.Key, parameterScalar->Value);
			}
			else if(const FKMParameterLinearColorValue* ParameterLinearColor = paramItr.Value.GetPtr<FKMParameterLinearColorValue>())
			{
				materialDyanmic->SetVectorParameterValue(paramItr.Key, ParameterLinearColor->Value);
			}
		}
	}
}

UKMParameterLayerSubsystem* UKMParameterLayerSubsystem::GetParameterLayerSubsystem(const UObject* worldContextObject)
{
	check(IsValid(worldContextObject) == true);
	
	UWorld* world = worldContextObject->GetWorld();
	if(!IsValid(world))
	{
		return nullptr;
	}
	return Cast<UKMParameterLayerSubsystem>(world->GetSubsystem<UKMParameterLayerSubsystem>());
}

void UKMParameterLayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UKMParameterLayerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKMParameterLayerSubsystem::OnComponentRemoved(UActorComponent* Component)
{
	if (USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(Component))
	{
		Systems.Remove(skeletalMeshComponent);
	}
}

TSharedPtr<FKMSkeletalMeshComponentParameterLayer> UKMParameterLayerSubsystem::GetSkeletalMeshComponentLayer(USkeletalMeshComponent* skeletalMeshComponent)
{
	if (!IsValid(skeletalMeshComponent))
	{
		return nullptr;
	}
	
	if (!skeletalMeshComponent->OnComponentDeactivated.IsAlreadyBound(this, &ThisClass::OnComponentRemoved))
	{
		skeletalMeshComponent->OnComponentDeactivated.AddDynamic(this, &ThisClass::OnComponentRemoved);
	}

	if (TSharedPtr<FKMSkeletalMeshComponentParameterLayer>* existParameterLayer = Systems.Find(skeletalMeshComponent))
	{
		if (existParameterLayer)
		{
			return *existParameterLayer;
		}
	}
	
	TSharedPtr<FKMSkeletalMeshComponentParameterLayer> newSkeletalMeshComponentParameterLayer = MakeShared<FKMSkeletalMeshComponentParameterLayer>(skeletalMeshComponent);
	newSkeletalMeshComponentParameterLayer->InitializeComponent();
	Systems.Emplace(skeletalMeshComponent, newSkeletalMeshComponentParameterLayer);
	return newSkeletalMeshComponentParameterLayer;
}


void UKMParameterLayerSubsystem::Tick(float DeltaTime)
{
	for (auto systemItr : Systems)
	{
		systemItr.Value->ApplyMaterialParameter();
	}
}

ETickableTickType UKMParameterLayerSubsystem::GetTickableTickType() const
{
	if (GetWorld())
	{
		if (GetWorld()->IsEditorWorld())
		{
			return Super::GetTickableTickType();
		}
	}
	return ETickableTickType::Never;
}

TStatId UKMParameterLayerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKMParameterLayerSubsystem, STATGROUP_Tickables)
}

bool UKMParameterLayerSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::Editor || WorldType == EWorldType::PIE || WorldType == EWorldType::EditorPreview;
}