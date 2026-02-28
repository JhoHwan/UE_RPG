// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SaveGame/MP2LoginSaveGame.h"

const FString UMP2LoginSaveGame::SaveSlotName{ TEXT("LoginData") };

UMP2LoginSaveGame::UMP2LoginSaveGame() : Email(TEXT("")), bRemember(false)
{
}

