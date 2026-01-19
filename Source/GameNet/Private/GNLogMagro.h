#include "GameNet.h"

#define GN_LOG(Format, ...) UE_LOG(GameNet, Log, TEXT(Format), ##__VA_ARGS__)
#define GN_WARN(Format, ...) UE_LOG(GameNet, Warning, TEXT(Format), ##__VA_ARGS__)
#define GN_ERR(Format, ...) UE_LOG(GameNet, Error, TEXT(Format), ##__VA_ARGS__)

#define GN_SCREENLOG(Format, ...) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT(Format), ##__VA_ARGS__)