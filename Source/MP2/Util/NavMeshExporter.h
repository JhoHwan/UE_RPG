// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavMeshExporter.generated.h"

#pragma pack(push, 4)
struct FStandardDetourTileHeader
{
	int magic;           
	int version;        
	int x;
	int y;
	int layer;
	unsigned int userId;
	int polyCount;
	int vertCount;
	int maxLinkCount;
	int detailMeshCount;
	int detailVertCount;
	int detailTriCount;
	int bvNodeCount;
	int offMeshConCount;
	int offMeshBase;
	float walkableHeight;
	float walkableRadius;
	float walkableClimb;
	float bmin[3];
	float bmax[3];
	float bvQuantFactor;  
};
#pragma pack(pop)

UCLASS()
class MP2_API ANavMeshExporter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANavMeshExporter();
	
	UPROPERTY(EditAnywhere, Category = "NavMesh Export")
	FString BinFileName = TEXT("NavMeshExport.bin");
	
	UFUNCTION(CallInEditor, Category = "NavMesh Export")
	void ExportNavMeshForServer();
};
