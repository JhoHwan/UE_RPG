// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPGPlayer.h"

// Sets default values
ARPGPlayer::ARPGPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARPGPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARPGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARPGPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

