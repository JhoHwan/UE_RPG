// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldPortal.h"

#include "Character/MP2Character.h"
#include "Components/BoxComponent.h"
#include "Network/ClientPacketHandler.h"
#include "Network/MP2NetSubsystem.h"
#include "Network/Protocol.pb.h"


// Sets default values
AFieldPortal::AFieldPortal()
{
	Collision = CreateDefaultSubobject<UBoxComponent>("Collision");
	RootComponent = Collision;
	Collision->SetBoxExtent({200.0, 200.0, 200.0});
}

void AFieldPortal::OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMP2Character* Character = Cast<AMP2Character>(OtherActor);
	if (!Character) return;
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Overlapped with %s"), *OtherActor->GetName()));
			
	Protocol::CS_USE_PORTAL pkt;
	pkt.set_portal_id(PortalId);
	UMP2NetSubsystem* NetSubsystem = GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	if (NetSubsystem)
	{
		NetSubsystem->RegisterSend(ClientPacketHandler::MakeSendBuffer(pkt));
	}
}

// Called when the game starts or when spawned
void AFieldPortal::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if (Collision)
		{
			Collision->OnComponentBeginOverlap.AddDynamic(this, &AFieldPortal::OnPortalOverlap);
		}
	}), 10.0f, false);

}

