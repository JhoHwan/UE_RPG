#include "MP2Editor.h"

DEFINE_LOG_CATEGORY(MP2Editor);

#define LOCTEXT_NAMESPACE "FMP2Editor"

void FMP2Editor::StartupModule()
{
	UE_LOG(MP2Editor, Warning, TEXT("MP2Editor module has been loaded"));
}

void FMP2Editor::ShutdownModule()
{
	UE_LOG(MP2Editor, Warning, TEXT("MP2Editor module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMP2Editor, MP2Editor)