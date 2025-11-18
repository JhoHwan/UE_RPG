// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SaveGame/MP2LoginSaveGame.h"

FString UMP2LoginSaveGame::SaveSlotName{ TEXT("RPGLoginSaveSlot") };

UMP2LoginSaveGame::UMP2LoginSaveGame() : Email(TEXT("")), bRememberEmail(false)
{
}
