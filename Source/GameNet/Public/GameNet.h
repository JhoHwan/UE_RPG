#pragma once

#include "Modules/ModuleManager.h"
#include "SendBuffer.h"

DECLARE_LOG_CATEGORY_EXTERN(GameNet, All, All)

struct PacketHeader
{
	uint16 size;
	uint16 id; 
};

class FGameNet : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;
};