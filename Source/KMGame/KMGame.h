#pragma once

#include "CoreMinimal.h"

class FKMGameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};