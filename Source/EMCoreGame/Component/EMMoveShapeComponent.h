#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "EMMoveShapeComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEMCollisionShapeType
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UENUM(Blueprintable, BlueprintType)
enum class EEMCollisionShapeType : uint8
{
	Capsule,
	Box,
	Sphere,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMMoveShapeComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Abstract)
class EMCOREGAME_API UEMMoveShapeComponent : public UCapsuleComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Shape)
	EEMCollisionShapeType CollisionShapeType = EEMCollisionShapeType::Capsule;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="CollisionShapeType==EEMCollisionShapeType::Box"), Category=Shape)
	FVector BoxExtent = FVector(32.f, 32.f, 32.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="CollisionShapeType==EEMCollisionShapeType::Sphere"), Category=Shape)
	float SphereRadius = 32.f;

public:
	UFUNCTION(BlueprintPure)
	float GetHorizontalDistance(const class UPrimitiveComponent* otherShapeComponent) const;

	UFUNCTION(BlueprintPure)
	float GetVerticalDistance(const class UPrimitiveComponent* otherShapeComponent) const;

	UFUNCTION(BlueprintPure)
	FVector GetScaledBoxExtent() const;

	UFUNCTION(BlueprintPure)
	float GetBottomLocationZ() const;

protected:
	virtual void UpdateBodySetup() override;
	virtual class FPrimitiveSceneProxy* CreateSceneProxy() override;
	
	virtual void OnRegister() override;

	float GetHorizontalRadiusInternal(const FVector& directionLocation) const;
	float GetVerticalExtentInternal() const;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& propertyChangedEvent) override;
#endif
};