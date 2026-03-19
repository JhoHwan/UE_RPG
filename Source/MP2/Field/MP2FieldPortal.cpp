// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2FieldPortal.h"

#include "Character/MP2Character.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Network/ClientPacketHandler.h"
#include "Network/MP2NetSubsystem.h"
#include "Network/Protocol.pb.h"


// Sets default values
AMP2FieldPortal::AMP2FieldPortal()
{
	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = Collision;
	Collision->SetSphereRadius(100.0f);
}

void AMP2FieldPortal::Interact(AMP2Character* Interactor)
{
	if (!Interactor->IsLocallyControlled()) return;
	
	Protocol::CS_USE_PORTAL Packet;
	Packet.set_portal_id(PortalId);
	if (UMP2NetSubsystem* NetSubsystem = GetGameInstance()->GetSubsystem<UMP2NetSubsystem>())
	{
		NetSubsystem->RegisterSend(ClientPacketHandler::MakeSendBuffer(Packet));
	}
}

FString AMP2FieldPortal::GetInteractText() const
{
	return TEXT("포탈 이동 (Space)");
}

void AMP2FieldPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMP2Character* Character = Cast<AMP2Character>(OtherActor);
	if (!Character || !Character->IsLocallyControlled()) return;
	
	Character->SetCurrentInteractable(this);
}

void AMP2FieldPortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMP2Character* Character = Cast<AMP2Character>(OtherActor);
	if (!Character || !Character->IsLocallyControlled()) return;
	
	Character->ClearCurrentInteractable(this);
}

// Called when the game starts or when spawned
void AMP2FieldPortal::BeginPlay()
{
	Super::BeginPlay();
	
	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AMP2FieldPortal::OnOverlapBegin);
		Collision->OnComponentEndOverlap.AddDynamic(this, &AMP2FieldPortal::OnOverlapEnd);
	}
}

