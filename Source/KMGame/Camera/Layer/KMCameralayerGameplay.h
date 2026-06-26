#pragma once

#include "CoreMinimal.h"
#include "KMCameralayerBase.h"
#include "KMCameralayerGameplay.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew, Abstract)
class KMGAME_API UKMCameralayerGameplay : public UKMCameralayerBase
{
	GENERATED_UCLASS_BODY()

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	FRotator TargetArmRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	FVector SocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	FVector TargetOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraCollision, meta=(editcondition="bDoCollisionTest"))
	float ProbeSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraCollision, meta=(editcondition="bDoCollisionTest"))
	TEnumAsByte<ECollisionChannel> ProbeChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraCollision)
	uint32 bDoCollisionTest:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraSettings)
	uint32 bUsePawnControlRotation:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraSettings)
	uint32 bInheritPitch : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraSettings)
	uint32 bInheritYaw : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraSettings)
	uint32 bInheritRoll : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag)
	uint32 bEnableCameraLag : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag)
	uint32 bEnableCameraRotationLag : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lag, AdvancedDisplay)
	uint32 bUseCameraLagSubstepping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag)
	uint32 bDrawDebugLagMarkers : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag, meta=(editcondition="bEnableCameraLag", ClampMin="0.0", ClampMax="1000.0", UIMin = "0.0", UIMax = "1000.0"))
	float CameraLagSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag, meta=(editcondition = "bEnableCameraRotationLag", ClampMin="0.0", ClampMax="1000.0", UIMin = "0.0", UIMax = "1000.0"))
	float CameraRotationLagSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lag, AdvancedDisplay, meta=(editcondition = "bUseCameraLagSubstepping", ClampMin="0.005", ClampMax="0.5", UIMin = "0.005", UIMax = "0.5"))
	float CameraLagMaxTimeStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag, meta=(editcondition="bEnableCameraLag", ClampMin="0.0", UIMin = "0.0"))
	float CameraLagMaxDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lag)
	uint32 bClampToMaxPhysicsDeltaTime : 1;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag")
	float HorizontalLeadDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag")
	float VerticalLeadDistance = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag")
	float ScreenLeadSpeed = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Lag")
	float ScreenReturnSpeed = 0.75f;

private:
	FVector CurrentLeadOffset = FVector::ZeroVector;
	
	bool bIsCameraFixed = false;
	FVector UnfixedCameraPosition;

	FVector PreviousDesiredLoc;
	FVector PreviousArmOrigin;
	FRotator PreviousDesiredRot;

	FVector RelativeSocketLocation;
	FQuat RelativeSocketRotation;

	FTransform CameraTransform;
	
public:
	static const FName SocketName;
	
public:
	virtual void Initalize() override;
	
protected:
	void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float deltaTime);
	FVector BlendLocations(const FVector& desiredArmLocation, const FVector& traceHitLocation, bool bHitSomething, float deltaTime);
	
	FVector GetUnfixedCameraPosition() const;
	bool IsCollisionFixApplied() const;

	FRotator GetTargetRotation() const;
	FRotator GetDesiredRotation() const;

	virtual void Evaluate(float deltaTime, FEMCameraOutput& output) override;
};