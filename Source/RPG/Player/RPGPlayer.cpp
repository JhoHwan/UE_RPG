// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPGPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera\CameraComponent.h"

ARPGPlayer::ARPGPlayer()
{
	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	CameraSpring->SetupAttachment(RootComponent);
	CameraSpring->TargetArmLength = 800.0f;
	CameraSpring->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));
	CameraSpring->bUsePawnControlRotation = false;
	CameraSpring->bInheritYaw = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpring, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void ARPGPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	APlayerController* PlayerController = Cast<APlayerController>(NewController);
	PlayerController->bShowMouseCursor = true;

	UEnhancedInputLocalPlayerSubsystem* SubSystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (SubSystem)
	{
		SubSystem->AddMappingContext(DefaultContext, 0);
	}
}

void ARPGPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGPlayer::Move);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ARPGPlayer::Zoom);


	}
}

void ARPGPlayer::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = CameraSpring->GetRelativeRotation();
	const FRotator YawRotation{ 0.0f, Rotation.Yaw, 0.0f };

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ARPGPlayer::Zoom(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	float NewCameraArmLength = CameraSpring->TargetArmLength;

	NewCameraArmLength += CameraZoomSpeed * InputValue;
	NewCameraArmLength = FMath::Clamp(NewCameraArmLength, CameraZoomInMax, CameraZoomOutMax);
	CameraSpring->TargetArmLength = NewCameraArmLength;
}

