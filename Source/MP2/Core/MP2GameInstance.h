// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MP2GameInstance.generated.h"

namespace Protocol
{
	class PlayerInfo;
}

class AMP2Character;
class UMP2FadeWidget;
/**
 * 
 */
UCLASS()
class MP2_API UMP2GameInstance : public UGameInstance
{
	GENERATED_BODY()
	friend class UMP2UISubSystem;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Level Trasition")
	void LoadLevelWithFade(FName NextLevelName);

protected:
	virtual void Init() override;
	virtual void Shutdown() override;
	
private:
	UFUNCTION()
	void ExecuteLevelLoad();
	
	void OnMapLoaded(UWorld* LoadedWorld);
	
private:
	UPROPERTY(EditAnywhere, Category="UI|Fade")
	TSubclassOf<UMP2FadeWidget> FadeWidgetClass;
	
	UPROPERTY(EditAnywhere, Category="Player")
	TSubclassOf<AMP2Character> OwnPlayerClass;
	
	FName LevelName;
};
