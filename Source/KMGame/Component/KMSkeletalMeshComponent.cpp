#include "KMSkeletalMeshComponent.h"

#include "KMAttachedBlendingComponent.h"
#include "Animation/KMAnimInstance.h"
#include "Core/KMParameterLayerSystem.h"

UKMSkeletalMeshComponent::UKMSkeletalMeshComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKMSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	OriginRelativeTransform = GetRelativeTransform();
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

void UKMSkeletalMeshComponent::AttachTransformUpdateComponent(UActorComponent* newActorComponent)
{
	if (!TransformUpdateComponentChilds.Contains(newActorComponent))
	{
		TransformUpdateComponentChilds.AddUnique(newActorComponent);
	}
}

void UKMSkeletalMeshComponent::DetachTransformUpdateComponent(UActorComponent* actorComponent)
{
	if (TransformUpdateComponentChilds.Contains(actorComponent))
	{
		TransformUpdateComponentChilds.Remove(actorComponent);
	}
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
	for (auto childComp : TransformUpdateComponentChilds)
	{
		if (!childComp.IsValid())
		{
			continue;
		}
		IEMTransformUpdateInterface* transformUpdateInterface = Cast<IEMTransformUpdateInterface>(childComp);
		if (!transformUpdateInterface)
		{
			continue;
		}
		transformUpdateInterface->UpdateTransform(GetWorld()->GetDeltaSeconds() * GetOwner()->CustomTimeDilation);
	}
	Super::FinalizeBoneTransform();
}

void UKMSkeletalMeshComponent::RevertOriginTransform()
{
	SetRelativeTransform(OriginRelativeTransform);

	if (UKMAnimInstance* followerAnimInstance = Cast<UKMAnimInstance>(GetAnimInstance()))
	{
		followerAnimInstance->UpdateAnimation(0.f, true);
	}
	UpdateComponentToWorld();
	RefreshBoneTransforms();
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