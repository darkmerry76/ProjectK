#include "KMSpringArmComponentVisualizer.h"
#include "Camera/KMSpringArmComponent.h"

void FKMSpringArmComponentVisualizer::DrawVisualization(const UActorComponent* component, const FSceneView* view, FPrimitiveDrawInterface* pDI)
{
	static const FColor	armColor(255,0,0);
	if (const UKMSpringArmComponent* springArm = Cast<const UKMSpringArmComponent>(component))
	{
		pDI->DrawLine( springArm->GetComponentLocation(), springArm->GetSocketTransform(UKMSpringArmComponent::SocketName,RTS_World).GetTranslation(), armColor, SDPG_World);
	}
}
