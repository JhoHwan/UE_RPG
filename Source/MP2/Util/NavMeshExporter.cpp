// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshExporter.h"

#include <fstream>

#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Runtime/Navmesh/Public/Detour/DetourNavMesh.h"


// Sets default values
ANavMeshExporter::ANavMeshExporter()
{
	PrimaryActorTick.bCanEverTick = false;
}

struct FNavMeshFileHeader
{
	int32 Magic;        // 파일 식별용 (0xDEADBEEF 등)
	int32 Version;      // 버전 관리용
	int32 TileCount;    // 저장된 타일 개수
	dtNavMeshParams Params; // 네비메시 설정 원본
};

void ANavMeshExporter::ExportNavMeshForServer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	//ARecastNavMesh* RecastNavMesh = Cast<ARecastNavMesh>(FNavigationSystem::GetCurrent)
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);

	if (!NavSys) return;
	ARecastNavMesh* RecastNavMesh = Cast<ARecastNavMesh>(NavSys->GetMainNavData());
	
	dtNavMesh* DetourMesh = RecastNavMesh->GetRecastMesh();
	if (!DetourMesh) return;

	auto params = DetourMesh->getParams();

	FString FilePath = FPaths::ProjectSavedDir() + BinFileName;
	std::ofstream os(TCHAR_TO_ANSI(*FilePath), std::ios::binary);
	if (!os.is_open()) return;

	FNavMeshFileHeader Header;
	Header.Magic = 0xDEADBEEF;
	Header.Version = 1;
	Header.TileCount = 0;
	Header.Params = *DetourMesh->getParams();

	const dtNavMesh* ConstMesh = DetourMesh;
	for (int i = 0; i < ConstMesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* Tile = ConstMesh->getTile(i);
		if (Tile && Tile->header && Tile->dataSize > 0)
		{
			Header.TileCount++;
		}
	}
	os.write((char*)&Header, sizeof(FNavMeshFileHeader));

	for (int i = 0; i < ConstMesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* Tile = ConstMesh->getTile(i);
		if (!Tile || !Tile->header || !Tile->dataSize) continue;

		os.write((char*)&Tile->dataSize, sizeof(int));

		os.write((char*)Tile->data, Tile->dataSize);
	}

	os.close();
	UE_LOG(LogTemp, Log, TEXT("NavMesh Saved: %d tiles to %s"), Header.TileCount, *FilePath);
}

