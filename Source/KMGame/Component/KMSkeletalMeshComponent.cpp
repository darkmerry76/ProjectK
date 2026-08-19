#include "KMSkeletalMeshComponent.h"
#include "Core/KMParameterLayerSystem.h"

UKMSkeletalMeshComponent::UKMSkeletalMeshComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKMSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKMSkeletalMeshComponent::OnRegister()
{
	Super::OnRegister();
	
	if (UKMParameterLayerSubsystem* parameterLayerSubsystem = UKMParameterLayerSubsystem::GetParameterLayerSubsystem(this))
	{
		TSharedPtr<FKMSkeletalMeshComponentParameterLayer> parameterLayer = parameterLayerSubsystem->GetSkeletalMeshComponentLayer(this);
		check(parameterLayer.IsValid());
	}
}

void UKMSkeletalMeshComponent::OnUnregister()
{
	Super::OnUnregister();
	
	if (UKMParameterLayerSubsystem* parameterLayerSubsystem = UKMParameterLayerSubsystem::GetParameterLayerSubsystem(this))
	{
		parameterLayerSubsystem->OnComponentRemoved(this);
	}
}

void UKMSkeletalMeshComponent::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void UKMSkeletalMeshComponent::SetMaterial(int32 elementIndex, UMaterialInterface* material)
{
	Super::SetMaterial(elementIndex, material);

	if (UKMParameterLayerSubsystem* parameterLayerSubsystem = UKMParameterLayerSubsystem::GetParameterLayerSubsystem(this))
	{
		TSharedPtr<FKMSkeletalMeshComponentParameterLayer> parameterLayer = parameterLayerSubsystem->GetSkeletalMeshComponentLayer(this);
		if (parameterLayer.IsValid())
		{
			parameterLayer->SetMaterial(elementIndex, material);
		}
	}
}

void UKMSkeletalMeshComponent::FinalizeBoneTransform()
{
	TArray<FTransform>& testTransform = GetEditableComponentSpaceTransforms();

	testTransform[0].SetLocation(FVector(0, 0, 0));

	Super::FinalizeBoneTransform();


	return;
}

void UKMSkeletalMeshComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	if (UKMParameterLayerSubsystem* parameterLayerSubsystem = UKMParameterLayerSubsystem::GetParameterLayerSubsystem(this))
	{
		TSharedPtr<FKMSkeletalMeshComponentParameterLayer> parameterLayer = parameterLayerSubsystem->GetSkeletalMeshComponentLayer(this);
		if (parameterLayer.IsValid())
		{
			parameterLayer->ApplyMaterialParameter();
		}
	}
}