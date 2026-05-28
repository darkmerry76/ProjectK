#pragma once

#include "CoreMinimal.h"
#include "KMUserWidget.h"
#include "KMCinematicWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMCinematicWidget : public UKMUserWidget
{
	GENERATED_BODY()
	
public:
	UKMCinematicWidget(const FObjectInitializer& objectInitializer);
	virtual bool Initialize() override;
};
