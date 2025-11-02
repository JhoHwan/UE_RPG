// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/RPGLoginSaveGame.h"

FString URPGLoginSaveGame::SaveSlotName{ TEXT("RPGLoginSaveSlot") };

URPGLoginSaveGame::URPGLoginSaveGame() : Email(TEXT("")), bRememberEmail(false)
{
}
