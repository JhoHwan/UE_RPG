// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MP2HumanCharacterBase.h"
#include "ActorComponents\MP2CharacterAppearanceComponent.h"
#include "Items\Data\Gear\MP2GearItemData.h"
//#include "GameNet\Public\GameNetSubsystem.h"

// Sets default values
AMP2HumanCharacterBase::AMP2HumanCharacterBase() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		SkinMaterialRef(TEXT("/Script/Engine.Material'/Game/Characters/Human/Material/M_Male_Skin.M_Male_Skin'"));
	if (SkinMaterialRef.Succeeded())
	{
		SkinMaterial = SkinMaterialRef.Object;
	}

	USkeletalMeshComponent* DefaultMesh = GetMesh();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> 
		FaceMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Characters/Human/Mesh/Face.Face'"));
	if (FaceMeshRef.Succeeded())
	{
		FaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FA"));
		FaceMesh->SetStaticMesh(FaceMeshRef.Object);
		FaceMesh->SetupAttachment(DefaultMesh, TEXT("Bip01-Head"));
		FaceMesh->SetRelativeLocation(FVector3d(-80.0, 0.0, 0.0));
		FaceMesh->SetRelativeRotation(FRotator(90.0, 90.0, -90.0));

	}

	HairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HR"));
	HairMesh->SetupAttachment(DefaultMesh, TEXT("Bip01-Head"));
	HairMesh->SetRelativeRotation(FRotator(90.0, 90.0, -90.0));

	CapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CP"));
	CapMesh->SetupAttachment(DefaultMesh, TEXT("Bip01-Head"));
	CapMesh->SetRelativeRotation(FRotator(90.0, 90.0, -90.0));

	ClothMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CL"));
	ClothMesh->SetupAttachment(DefaultMesh);
	ClothMesh->SetLeaderPoseComponent(DefaultMesh);
		
	PantsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PA"));
	PantsMesh->SetupAttachment(DefaultMesh);
	PantsMesh->SetLeaderPoseComponent(DefaultMesh);

	GlovesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GA"));
	GlovesMesh->SetupAttachment(DefaultMesh);
	GlovesMesh->SetLeaderPoseComponent(DefaultMesh);

	ShoesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SH"));
	ShoesMesh->SetupAttachment(DefaultMesh);
	ShoesMesh->SetLeaderPoseComponent(DefaultMesh);

	AppearanceComponent = CreateDefaultSubobject<UMP2CharacterAppearanceComponent>(TEXT("AppearanceComponent"));
}

void AMP2HumanCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AppearanceComponent)
	{
		FMP2CharacterPartComponents PartComponents;

		PartComponents.HairMesh = HairMesh;
		PartComponents.FaceMesh = FaceMesh;
		PartComponents.CapMesh = CapMesh;
		PartComponents.SkinMaterial = SkinMaterial;
		PartComponents.ClothMesh = ClothMesh;
		PartComponents.PantsMesh = PantsMesh;
		PartComponents.GlovesMesh = GlovesMesh;
		PartComponents.ShoesMesh = ShoesMesh;

		AppearanceComponent->InitializeParts(PartComponents);
	}
}

// Called when the game starts or when spawned
void AMP2HumanCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}