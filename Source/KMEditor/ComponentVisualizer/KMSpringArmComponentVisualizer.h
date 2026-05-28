#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"

class KMEDITOR_API FKMSpringArmComponentVisualizer : public FComponentVisualizer
{
public:
	virtual void DrawVisualization(const class UActorComponent* component, const class FSceneView* view, class FPrimitiveDrawInterface* pDI) override;
};
