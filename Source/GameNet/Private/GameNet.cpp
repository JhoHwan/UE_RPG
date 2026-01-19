#include "GameNet.h"

DEFINE_LOG_CATEGORY(GameNet);

#define LOCTEXT_NAMESPACE "FGameNet"

void FGameNet::StartupModule()
{
	UE_LOG(GameNet, Warning, TEXT("GameNet module has been loaded"));
}

void FGameNet::ShutdownModule()
{
	UE_LOG(GameNet, Warning, TEXT("GameNet module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameNet, GameNet)