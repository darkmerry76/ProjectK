#include "KMChainAnimInstance.h"
#include "Animation/AnimInstanceProxy.h"

FKMChainAnimInstanceProxy::FKMChainAnimInstanceProxy(UAnimInstance* instance) : FAnimInstanceProxy(instance)
{
}

void FKMChainAnimInstanceProxy::PreUpdate(UAnimInstance* animInstance, float deltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(animInstance, deltaSeconds);

	const UKMChainAnimInstance* castAnimInstance = static_cast<const UKMChainAnimInstance*>(animInstance);
	check(IsValid(castAnimInstance));
	
	Snapshot = castAnimInstance->Snapshot;
	BlendAlpha = castAnimInstance->BlendAlpha;
	EnableAttack = castAnimInstance->EnableAttack;
	TargetLocation = castAnimInstance->TargetLocation;
	CustomTimeDilation = castAnimInstance->CustomTimeDilation;
}

const FPoseSnapshot& FKMChainAnimInstanceProxy::GetPoseSnapshot() const
{
	return Snapshot;
}

float FKMChainAnimInstanceProxy::GetBlendAlpha() const
{
	return BlendAlpha;
}

bool FKMChainAnimInstanceProxy::IsEnableAttack() const
{
	return EnableAttack;
}

const FVector& FKMChainAnimInstanceProxy::GetTargetLocation() const
{
	return TargetLocation;
}

float FKMChainAnimInstanceProxy::GetCustomTimeDilation() const
{
	return CustomTimeDilation;
}

void UKMChainAnimInstance::SetTargetLocation(const FVector& newTargetlocation)
{
	TargetLocation = newTargetlocation;
}

FAnimInstanceProxy* UKMChainAnimInstance::CreateAnimInstanceProxy()
{
	return new FKMChainAnimInstanceProxy(this);
}

void UKMChainAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);

	if (AActor* ownerActor = GetTypedOuter<AActor>())
	{
		CustomTimeDilation = ownerActor->CustomTimeDilation;
	}
	
}