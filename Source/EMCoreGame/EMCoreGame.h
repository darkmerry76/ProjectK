#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FEMCoreGame : public IModuleInterface
{
public:
	FEMCoreGame();
	virtual ~FEMCoreGame() override;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
