#pragma once

#include "Modules/ModuleManager.h"

class FDetexModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
