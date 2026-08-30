#include "EMMoveShapeComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/SphereElem.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMMoveShapeComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UEMMoveShapeComponent::UEMMoveShapeComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UEMMoveShapeComponent::OnRegister()
{
	Super::OnRegister();

	if (CollisionShapeType == EEMCollisionShapeType::Box)
	{
		CapsuleHalfHeight = BoxExtent.Z;
	}
	else if (CollisionShapeType == EEMCollisionShapeType::Sphere)
	{
		CapsuleHalfHeight = SphereRadius;	
	}
}

FPrimitiveSceneProxy* UEMMoveShapeComponent::CreateSceneProxy()
{
	class FDrawCylinderSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		FDrawCylinderSceneProxy(const UEMMoveShapeComponent* moveComponent) : FPrimitiveSceneProxy(moveComponent)
			, CollisionShapeType(moveComponent->CollisionShapeType)
			, bDrawOnlyIfSelected(moveComponent->bDrawOnlyIfSelected)
			, CapsuleRadius(moveComponent->GetScaledCapsuleRadius())
			, CapsuleHalfHeight( moveComponent->GetScaledCapsuleHalfHeight())
			, BoxExtent(moveComponent->BoxExtent)
			, SphereRadius(moveComponent->SphereRadius)
			, ShapeColor(moveComponent->ShapeColor)
			, LineThickness(moveComponent->LineThickness)
		{
			bWillEverBeLit = false;
		}
	protected:
		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& views, const FSceneViewFamily& viewFamily, uint32 visibilityMap, FMeshElementCollector& collector) const override
		{
			if (CollisionShapeType == EEMCollisionShapeType::Capsule)
			{
				GetDynamicCapsuleMeshElements(views, viewFamily, visibilityMap, collector);
			}
			else if (CollisionShapeType == EEMCollisionShapeType::Box)
			{
				GetDynamicBoxMeshElements(views, viewFamily, visibilityMap, collector);
			}
			else if (CollisionShapeType == EEMCollisionShapeType::Sphere)
			{
				GetDynamicSphereMeshElements(views, viewFamily, visibilityMap, collector);
			}
		}

				void GetDynamicCapsuleMeshElements(const TArray<const FSceneView*>& views, const FSceneViewFamily& viewFamily, uint32 visibilityMap, FMeshElementCollector& collector) const
		{
			QUICK_SCOPE_CYCLE_COUNTER( STAT_GetDynamicMeshElements_DrawDynamicElements );

			const FMatrix& localToWorld = GetLocalToWorld();
			const int32 capsuleSides =  FMath::Clamp<int32>(CapsuleRadius / 4.f, 16, 64);

			for (int32 viewIndex = 0; viewIndex < views.Num(); ++viewIndex)
			{
				if (visibilityMap & (1 << viewIndex))
				{
					const FSceneView* view = views[viewIndex];
					const FLinearColor drawCapsuleColor = GetViewSelectionColor(ShapeColor, *view, IsSelected(), IsHovered(), false, IsIndividuallySelected());
					FPrimitiveDrawInterface* pDI = collector.GetPDI(viewIndex);
					DrawWireCapsule(pDI, localToWorld.GetOrigin(),
						localToWorld.GetUnitAxis(EAxis::X), localToWorld.GetUnitAxis(EAxis::Y), localToWorld.GetUnitAxis(EAxis::Z),
						drawCapsuleColor, CapsuleRadius, CapsuleHalfHeight, capsuleSides, SDPG_World, LineThickness);
				}
			}
		}

		void GetDynamicBoxMeshElements(const TArray<const FSceneView*>& views, const FSceneViewFamily& viewFamily, uint32 visibilityMap, FMeshElementCollector& collector) const
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_BoxSceneProxy_GetDynamicMeshElements);

			const FMatrix& localToWorld = GetLocalToWorld();
			
			for (int32 viewIndex = 0; viewIndex < views.Num(); ++viewIndex)
			{
				if (visibilityMap & (1 << viewIndex))
				{
					const FSceneView* view = views[viewIndex];
					const FLinearColor drawBoxColor = GetViewSelectionColor(ShapeColor, *view, IsSelected(), IsHovered(), false, IsIndividuallySelected() );
					FPrimitiveDrawInterface* pDI = collector.GetPDI(viewIndex);
					DrawOrientedWireBox(pDI, localToWorld.GetOrigin(),
						localToWorld.GetScaledAxis( EAxis::X ), localToWorld.GetScaledAxis( EAxis::Y ), localToWorld.GetScaledAxis( EAxis::Z ), BoxExtent, drawBoxColor, SDPG_World, LineThickness);
				}
			}
		}

		void GetDynamicSphereMeshElements(const TArray<const FSceneView*>& views, const FSceneViewFamily& viewFamily, uint32 visibilityMap, FMeshElementCollector& collector) const
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_SphereSceneProxy_GetDynamicMeshElements);

			for (int32 viewIndex = 0; viewIndex < views.Num(); ++viewIndex)
			{
				if (visibilityMap & (1 << viewIndex))
				{
					const FSceneView* view = views[viewIndex];
					FPrimitiveDrawInterface* pDI = collector.GetPDI(viewIndex);

					const FMatrix& localToWorld = GetLocalToWorld();
					const FLinearColor drawSphereColor = GetViewSelectionColor(ShapeColor, *view, IsSelected(), IsHovered(), false, IsIndividuallySelected() );

					const float distanceSqr = (view->ViewMatrices.GetViewOrigin() - localToWorld.GetOrigin()).SizeSquared();
					if (distanceSqr < FMath::Square(GetMinDrawDistance()) || distanceSqr > FMath::Square(GetMaxDrawDistance()) )
					{
						continue;
					}
					
					float absScaleX = localToWorld.GetScaledAxis(EAxis::X).Size();
					float absScaleY = localToWorld.GetScaledAxis(EAxis::Y).Size();
					float absScaleZ = localToWorld.GetScaledAxis(EAxis::Z).Size();
					float minAbsScale = FMath::Min3(absScaleX, absScaleY, absScaleZ);

					FVector scaledX = localToWorld.GetUnitAxis(EAxis::X) * minAbsScale;
					FVector scaledY = localToWorld.GetUnitAxis(EAxis::Y) * minAbsScale;
					FVector scaledZ = localToWorld.GetUnitAxis(EAxis::Z) * minAbsScale;

					const int32 sphereSides = FMath::Clamp<int32>(SphereRadius / 4.f, 16, 64);
					DrawCircle(pDI, localToWorld.GetOrigin(), scaledX, scaledY, drawSphereColor, SphereRadius, sphereSides, SDPG_World, LineThickness);
					DrawCircle(pDI, localToWorld.GetOrigin(), scaledX, scaledZ, drawSphereColor, SphereRadius, sphereSides, SDPG_World, LineThickness);
					DrawCircle(pDI, localToWorld.GetOrigin(), scaledY, scaledZ, drawSphereColor, SphereRadius, sphereSides, SDPG_World, LineThickness);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* view) const override
		{
			const bool bProxyVisible = !bDrawOnlyIfSelected || IsSelected();
			const bool bShowForCollision = view->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance result;
			result.bDrawRelevance = (IsShown(view) && bProxyVisible) || bShowForCollision;
			result.bDynamicRelevance = true;
			result.bShadowRelevance = IsShadowCast(view);
			result.bEditorPrimitiveRelevance = UseEditorCompositing(view);
			return result;
		}
		
		virtual SIZE_T GetTypeHash() const override
		{
			static size_t uniquePointer;
			return reinterpret_cast<size_t>(&uniquePointer);
		}
		
		virtual uint32 GetMemoryFootprint() const override { return (sizeof(*this) + GetAllocatedSize() ); }

	private:
		const EEMCollisionShapeType CollisionShapeType;
		const uint32 bDrawOnlyIfSelected:1;
		const float	CapsuleRadius;
		const float CapsuleHalfHeight;
		const FVector BoxExtent;
		const float SphereRadius;
		const FColor ShapeColor;
		const float LineThickness;
	};

	return new FDrawCylinderSceneProxy(this);
}

template <EShapeBodySetupHelper UpdateBodySetupAction, typename BodySetupType>
bool InvalidateOrUpdateCapsuleBodySetup(BodySetupType& shapeBodySetup, bool bUseArchetypeBodySetup, float capsuleRadius, float capsuleHalfHeight)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup));
	check(shapeBodySetup->AggGeom.SphylElems.Num() == 1);
	FKSphylElem* sE = shapeBodySetup->AggGeom.SphylElems.GetData();

	const float length = 2 * FMath::Max(capsuleHalfHeight - capsuleRadius, 0.f);
	if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		sE->SetTransform(FTransform::Identity);
		sE->Radius = capsuleRadius;
		sE->Length = length;
	}
	else
	{
		if(sE->Radius != capsuleRadius || sE->Length != length)
		{
			shapeBodySetup = nullptr;
			bUseArchetypeBodySetup = false;
		}
	}
	
	return bUseArchetypeBodySetup;
}

template <EShapeBodySetupHelper UpdateBodySetupAction, typename BodySetupType>
bool InvalidateOrUpdateBoxBodySetup(BodySetupType& shapeBodySetup, bool bUseArchetypeBodySetup, FVector boxExtent)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup));
	check(shapeBodySetup->AggGeom.BoxElems.Num() == 1);
	FKBoxElem* se = shapeBodySetup->AggGeom.BoxElems.GetData();

	if (boxExtent.X < UE_KINDA_SMALL_NUMBER)
	{
		boxExtent.X = 1.0f;
	}

	if (boxExtent.Y < UE_KINDA_SMALL_NUMBER)
	{
		boxExtent.Y = 1.0f;
	}

	if (boxExtent.Z < UE_KINDA_SMALL_NUMBER)
	{
		boxExtent.Z = 1.0f;
	}

	float xExtent = boxExtent.X * 2.f;
	float yExtent = boxExtent.Y * 2.f;
	float zExtent = boxExtent.Z * 2.f;

	if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		se->SetTransform(FTransform::Identity);
		se->X = xExtent;
		se->Y = yExtent;
		se->Z = zExtent;
	}
	else if(se->X != xExtent || se->Y != yExtent || se->Z != zExtent)
	{
		shapeBodySetup = nullptr;
		bUseArchetypeBodySetup = false;
	}
	return bUseArchetypeBodySetup;
}

template <EShapeBodySetupHelper UpdateBodySetupAction, typename BodySetupType>
bool InvalidateOrUpdateSphereBodySetup(BodySetupType& shapeBodySetup, bool bUseArchetypeBodySetup, float sphereRadius)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup) );
	check(shapeBodySetup->AggGeom.SphereElems.Num() == 1);
	FKSphereElem* sphereElem = shapeBodySetup->AggGeom.SphereElems.GetData();

	float radius = sphereRadius;
	if (radius < UE_KINDA_SMALL_NUMBER)
	{
		radius = 0.1f;
	}

	if(UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		sphereElem->Center = FVector::ZeroVector;
		sphereElem->Radius = radius;
	}
	else
	{
		if(sphereElem->Radius != radius)
		{
			shapeBodySetup = nullptr;
			bUseArchetypeBodySetup = false;
		}
	}
	return bUseArchetypeBodySetup;
}

void UEMMoveShapeComponent::UpdateBodySetup()
{
	if (CollisionShapeType == EEMCollisionShapeType::Capsule)
	{
		if (PrepareSharedBodySetup<UEMMoveShapeComponent>())
		{
			bUseArchetypeBodySetup = InvalidateOrUpdateCapsuleBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, CapsuleRadius, CapsuleHalfHeight);
		}

		CreateShapeBodySetupIfNeeded<FKSphylElem>();

		if (!bUseArchetypeBodySetup)
		{
			InvalidateOrUpdateCapsuleBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, CapsuleRadius, CapsuleHalfHeight);
		}
	}
	else if (CollisionShapeType == EEMCollisionShapeType::Box)
	{
		if (PrepareSharedBodySetup<UEMMoveShapeComponent>())
		{
			bUseArchetypeBodySetup = InvalidateOrUpdateBoxBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, BoxExtent);
		}
		
		CreateShapeBodySetupIfNeeded<FKBoxElem>();
		
		if (!bUseArchetypeBodySetup)
		{
			InvalidateOrUpdateBoxBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, BoxExtent);
		}
	}
	else if (CollisionShapeType == EEMCollisionShapeType::Sphere)
	{
		if (PrepareSharedBodySetup<UEMMoveShapeComponent>())
		{
			bUseArchetypeBodySetup = InvalidateOrUpdateSphereBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, SphereRadius);
		}
		
		CreateShapeBodySetupIfNeeded<FKSphereElem>();

		if(!bUseArchetypeBodySetup)
		{
			InvalidateOrUpdateSphereBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, SphereRadius);
		}
	}
}

FVector UEMMoveShapeComponent::GetScaledBoxExtent() const
{
	return BoxExtent * GetComponentScale();
}

float UEMMoveShapeComponent::GetHorizontalRadiusInternal(const FVector& directionLocation) const
{
	if (CollisionShapeType == EEMCollisionShapeType::Box)
	{
		FVector direction = directionLocation - GetComponentLocation();
		direction.Z = 0.f;
		direction.Normalize();

		const FVector boxExtent = GetScaledBoxExtent();

		return  FMath::Abs(FVector::DotProduct(direction, GetForwardVector())) * boxExtent.X +
				FMath::Abs(FVector::DotProduct(direction, GetRightVector())) * boxExtent.Y +
				FMath::Abs(FVector::DotProduct(direction, GetUpVector())) * boxExtent.Z;
	}

	return GetScaledCapsuleRadius();
}

float UEMMoveShapeComponent::GetHorizontalDistance(const UPrimitiveComponent* otherShapeComponent) const
{
	const FVector otherLocation = otherShapeComponent->GetComponentLocation();
	
	float distance = (otherLocation - GetComponentLocation()).Size2D();
	distance -= GetHorizontalRadiusInternal(otherLocation);

	if (const UEMMoveShapeComponent* otherMoveShapeComponent = Cast<UEMMoveShapeComponent>(otherShapeComponent))
	{
		distance -= otherMoveShapeComponent->GetHorizontalRadiusInternal(GetComponentLocation());
	}
	else if (const UCapsuleComponent* otherCapsuleComponent = Cast<UCapsuleComponent>(otherShapeComponent))
	{
		distance -= otherCapsuleComponent->GetScaledCapsuleRadius();
	}

	return distance;
}

float UEMMoveShapeComponent::GetVerticalExtentInternal() const
{
	if (CollisionShapeType == EEMCollisionShapeType::Box)
	{
		const FVector boxExtent = GetScaledBoxExtent();
		return	FMath::Abs(GetForwardVector().Z) * boxExtent.X +
				FMath::Abs(GetRightVector().Z) * boxExtent.Y +
				FMath::Abs(GetUpVector().Z) * boxExtent.Z;
	}

	return GetScaledCapsuleHalfHeight();
}

float UEMMoveShapeComponent::GetBottomLocationZ() const
{
	return GetComponentLocation().Z - GetVerticalExtentInternal();
}

float UEMMoveShapeComponent::GetVerticalDistance(const UPrimitiveComponent* otherShapeComponent) const
{
	float otherBottomLocationZ = otherShapeComponent->GetComponentLocation().Z;

	if (const UEMMoveShapeComponent* otherMoveShapeComponent = Cast<UEMMoveShapeComponent>(otherShapeComponent))
	{
		otherBottomLocationZ = otherMoveShapeComponent->GetBottomLocationZ();
	}
	else if (const UCapsuleComponent* otherCapsuleComponent = Cast<UCapsuleComponent>(otherShapeComponent))
	{
		otherBottomLocationZ -= otherCapsuleComponent->GetScaledCapsuleHalfHeight();
	}

	return FMath::Abs(GetBottomLocationZ() - otherBottomLocationZ);
}

#if WITH_EDITOR
void UEMMoveShapeComponent::PostEditChangeProperty(struct FPropertyChangedEvent& propertyChangedEvent)
{
	if (propertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEMMoveShapeComponent, CollisionShapeType))
	{
		ShapeBodySetup = nullptr;
		bUseArchetypeBodySetup = false;
		UpdateBodySetup();
	}
	else if (propertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEMMoveShapeComponent, BoxExtent))
	{
		CapsuleHalfHeight = BoxExtent.Z;
	}
	else if (propertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEMMoveShapeComponent, SphereRadius))
	{
		CapsuleHalfHeight = SphereRadius;
	}
}
#endif