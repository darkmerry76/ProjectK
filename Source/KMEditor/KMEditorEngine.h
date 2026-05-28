#pragma once

#include "CoreMinimal.h"
#include "Editor/EditorEngine.h"
#include "Editor/UnrealEdEngine.h"
#include "KMEditorEngine.generated.h"

UCLASS()
class KMEDITOR_API UKMEditorEngine : public UUnrealEdEngine
{
	GENERATED_BODY()
public:
	virtual void Init(IEngineLoop* InEngineLoop) override;
};
