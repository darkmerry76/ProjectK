#pragma once

#include "CoreMinimal.h"

class FKMEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};