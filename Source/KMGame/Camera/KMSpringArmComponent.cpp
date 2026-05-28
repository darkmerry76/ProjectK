#include "KMSpringArmComponent.h"

#include "KMCameraActorBase.h"
#include "Character/KMCharacter.h"
#include "PhysicsEngine/PhysicsSettings.h"

const FName UKMSpringArmComponent::SocketName(TEXT("SpringEndpoint"));

UKMSpringArmComponent::UKMSpringArmComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bUsePawnControlRotation = false;
	bDoCollisionTest = true;

	bInheritPitch = true;
	bInheritYaw = true;
	bInheritRoll = true;

	TargetArmLength = 300.0f;
	ProbeSize = 12.0f;
	ProbeChannel = ECC_Camera;

	RelativeSocketRotation = FQuat::Identity;

	bUseCameraLagSubstepping = true;
	CameraLagSpeed = 10.f;
	CameraRotationLagSpeed = 10.f;
	CameraLagMaxTimeStep = 1.f / 60.f;
	CameraLagMaxDistance = 0.f;
	bClampToMaxPhysicsDeltaTime = false;

	UnfixedCameraPosition = FVector::ZeroVector;
}

void UKMSpringArmComponent::OnRegister()
{
	Super::OnRegister();

	CameraLagMaxTimeStep = FMath::Max(CameraLagMaxTimeStep, 1.f / 200.f);
	CameraLagSpeed = FMath::Max(CameraLagSpeed, 0.f);

	UpdateDesiredArmLocation(false, false, false, 0.f);
}

void UKMSpringArmComponent::PostLoad()
{
	Super::PostLoad();
}

FVector UKMSpringArmComponent::GetUnfixedCameraPosition() const
{
	return UnfixedCameraPosition;
}

bool UKMSpringArmComponent::IsCollisionFixApplied() const
{
	return bIsCameraFixed;
}

FRotator UKMSpringArmComponent::GetDesiredRotation() const
{
	return GetComponentRotation();
}

FRotator UKMSpringArmComponent::GetTargetRotation() const
{
	FRotator DesiredRot = GetDesiredRotation();

	if (bUsePawnControlRotation)
	{
		if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
		{
			const FRotator PawnViewRotation = OwningPawn->GetViewRotation();
			if (DesiredRot != PawnViewRotation)
			{
				DesiredRot = PawnViewRotation;
			}
		}
	}
	
	if (!IsUsingAbsoluteRotation())
	{
		const FRotator LocalRelativeRotation = GetRelativeRotation();
		if (!bInheritPitch)
		{
			DesiredRot.Pitch = LocalRelativeRotation.Pitch;
		}

		if (!bInheritYaw)
		{
			DesiredRot.Yaw = LocalRelativeRotation.Yaw;
		}

		if (!bInheritRoll)
		{
			DesiredRot.Roll = LocalRelativeRotation.Roll;
		}
	}

	return DesiredRot;
}

FVector UKMSpringArmComponent::BlendLocations(const FVector& desiredArmLocation, const FVector& traceHitLocation, bool bHitSomething, float deltaTime)
{
	return bHitSomething ? traceHitLocation : desiredArmLocation;
}

void UKMSpringArmComponent::UpdateDesiredArmLocation(
    bool bDoTrace,
    bool bDoLocationLag,
    bool bDoRotationLag,
    float deltaTime)
{
    FRotator desiredRot = GetTargetRotation();

    if (bDoRotationLag)
    {
       desiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(desiredRot), deltaTime, CameraRotationLagSpeed));
    }
    PreviousDesiredRot = desiredRot;

    FVector armOrigin = GetComponentLocation() + TargetOffset;

    FVector targetLeadOffset = FVector::ZeroVector;

    if (AKMCameraActorBase* owner = Cast<AKMCameraActorBase>(GetOwner()))
    {
        if (AKMCharacter* targetCharacter = Cast<AKMCharacter>(owner->GetTargetActor()))
        {
           FVector velocity = targetCharacter->GetInputVelocity();
           velocity.Z = 0.f;

           if (velocity.SizeSquared() > 0.001f)
           {
              FVector moveDirection = velocity.GetSafeNormal();
              FMatrix cameraRotationMatrix = FRotationMatrix(desiredRot);
              FVector cameraRight = cameraRotationMatrix.GetUnitAxis(EAxis::Y); 
              
              FVector cameraUp = cameraRotationMatrix.GetUnitAxis(EAxis::Z);
              FVector flatForward = cameraRotationMatrix.GetUnitAxis(EAxis::X);
              flatForward.Z = 0.f;
              flatForward.Normalize();

              float dotRight = FVector::DotProduct(moveDirection, cameraRight);
              float dotForward = FVector::DotProduct(moveDirection, flatForward);

              targetLeadOffset += cameraRight * (dotRight * HorizontalLeadDistance);

              targetLeadOffset += flatForward * (dotForward * VerticalLeadDistance);

              CurrentLeadOffset = FMath::VInterpTo(CurrentLeadOffset, targetLeadOffset, deltaTime, ScreenLeadSpeed);
           }
           else
           {
              CurrentLeadOffset = FMath::VInterpTo(CurrentLeadOffset, FVector::ZeroVector, deltaTime, ScreenReturnSpeed);
           }
        }
    }

    FVector desiredLoc = armOrigin;

    PreviousArmOrigin = armOrigin;
    PreviousDesiredLoc = desiredLoc;

    desiredLoc -= desiredRot.Vector() * TargetArmLength;
    desiredLoc += FRotationMatrix(desiredRot).TransformVector(SocketOffset);

    FVector resultLoc = desiredLoc;

    if (bDoTrace && TargetArmLength != 0.0f)
    {
       bIsCameraFixed = true;
       FCollisionQueryParams queryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());
       FHitResult hitResult;

       GetWorld()->SweepSingleByChannel(hitResult, armOrigin,
       	desiredLoc, FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), queryParams);

       UnfixedCameraPosition = desiredLoc;
       resultLoc = BlendLocations(desiredLoc, hitResult.Location, hitResult.bBlockingHit, deltaTime);

       if (resultLoc == desiredLoc)
       {
          bIsCameraFixed = false;
       }
    }
    else
    {
       bIsCameraFixed = false;
       UnfixedCameraPosition = resultLoc;
    }

	resultLoc += CurrentLeadOffset;

    CameraTransform.SetLocation(resultLoc);
    CameraTransform.SetRotation(desiredRot.Quaternion());

    FTransform worldCamTM(desiredRot, resultLoc);
    FTransform relCamTM = worldCamTM.GetRelativeTransform(GetComponentTransform());

    RelativeSocketLocation = relCamTM.GetLocation();
    RelativeSocketRotation = relCamTM.GetRotation();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
    if (bDrawDebugLagMarkers)
    {
       DrawDebugSphere(GetWorld(), armOrigin, 8.f, 8, FColor::Green);
       DrawDebugSphere(GetWorld(), resultLoc, 12.f, 8, FColor::Cyan);
       DrawDebugLine(GetWorld(), armOrigin, resultLoc, FColor::Yellow, false, 0.f, 0, 2.f);
    }
#endif

    UpdateChildTransforms();
}

FTransform UKMSpringArmComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	FTransform RelativeTransform(RelativeSocketRotation, RelativeSocketLocation);

	switch(TransformSpace)
	{
	case RTS_World:
		{
			return RelativeTransform * GetComponentTransform();
		}
	case RTS_Actor:
		{
			if( const AActor* Actor = GetOwner() )
			{
				FTransform SocketTransform = RelativeTransform * GetComponentTransform();
				return SocketTransform.GetRelativeTransform(Actor->GetTransform());
			}
			break;
		}
	case RTS_Component:
		{
			return RelativeTransform;
		}
	default:break;
	}
	return RelativeTransform;
}

bool UKMSpringArmComponent::HasAnySockets() const
{
	return true;
}

void UKMSpringArmComponent::ApplyWorldOffset(const FVector & InOffset, bool bWorldShift)
{
	Super::ApplyWorldOffset(InOffset, bWorldShift);
	PreviousDesiredLoc += InOffset;
	PreviousArmOrigin += InOffset;
}

void UKMSpringArmComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const
{
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

void UKMSpringArmComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	UpdateDesiredArmLocation(bDoCollisionTest, bEnableCameraLag, bEnableCameraRotationLag, deltaTime);	
}