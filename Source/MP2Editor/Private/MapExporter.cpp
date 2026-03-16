// Fill out your copyright notice in the Description page of Project Settings.


#include "MapExporter.h"

#include "AssetExportTask.h"
#include "MP2Editor.h"
#include "NavigationSystem.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshConversion.h"
#include "AI/NavigationSystemBase.h"
#include "Exporters/Exporter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/MonitoredProcess.h"
#include "NavMesh/RecastNavMesh.h"
#include "Field/FieldPortal.h"

#include "Field/MP2FieldManifestDataAsset.h"
#include "Core/Setting/MP2NetworkObjectSettings.h"

bool UMapExporter::ExportAllMaps()
{
	UE_LOG(LogTemp, Log, TEXT("Starting ExportAllMaps..."));

	const UMP2NetworkObjectSettings* Settings = GetDefault<UMP2NetworkObjectSettings>();
	if (!Settings) return false;

	UMP2FieldManifestDataAsset* Manifest = Settings->FieldManifestData.LoadSynchronous();
	if (!Manifest)
	{
		UE_LOG(LogTemp, Error, TEXT("FieldManifestData is not set in NetworkObjectSettings."));
		return false;
	}

	TMap<FGuid, TPair<int32, int32>> PortalGuidToInfoMap;

	UE_LOG(LogTemp, Log, TEXT("Phase 1: Caching portal GUIDs and MapIds..."));
	// Phase 1: 전수 조사를 통한 포탈 GUID와 MapId 매핑 캐싱
	for (const auto& Pair : Manifest->FieldDats)
	{
		int32 MapId = Pair.Key;
		UMP2FieldDataAsset* FieldData = Pair.Value;
		if (!FieldData) continue;

		UWorld* World = FieldData->MapAsset.LoadSynchronous();
		if (!World) continue;

		TArray<AActor*> FieldPortals;
		UGameplayStatics::GetAllActorsOfClass(World, AFieldPortal::StaticClass(), FieldPortals);
		for (int32 i = 0; i < FieldPortals.Num(); ++i)
		{
			AFieldPortal* Portal = Cast<AFieldPortal>(FieldPortals[i]);
			if (Portal)
			{
				Portal->PortalId = i;
				Portal->MarkPackageDirty();
				PortalGuidToInfoMap.Add(Portal->MyGuid, TPair<int32, int32>(MapId, i));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Phase 2: Exporting map data and navmesh..."));
	// Phase 2: 실제 데이터 추출 및 익스포트
	int32 ExportCount = 0;
	for (const auto& Pair : Manifest->FieldDats)
	{
		int32 MapId = Pair.Key;
		UMP2FieldDataAsset* FieldData = Pair.Value;
		if (!FieldData) continue;

		UWorld* World = FieldData->MapAsset.LoadSynchronous();
		if (!World) continue;

		ExportMapDataJson(MapId, FieldData->MapName, World, PortalGuidToInfoMap);
		
		// 테스트를 위해 V2 호출
		if (ExportNavMeshBinV2(FieldData->MapName, World))
		{
			UE_LOG(LogTemp, Log, TEXT("Map %d (%s) processed with NavMesh V2."), MapId, *FieldData->MapName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Map %d (%s) NavMesh V2 export failed."), MapId, *FieldData->MapName);
		}

		ExportCount++;
	}

	UE_LOG(LogTemp, Log, TEXT("ExportAllMaps completed. Processed %d maps."), ExportCount);
	return true;
}

bool UMapExporter::ExportNavMeshBinV2(const FString& MapName, UWorld* World)
{
	UStaticMesh* StaticMesh = NavMeshToStaticMeshV2(World);
	if (!StaticMesh) return false;
	ExportStaticMeshToBin(StaticMesh, MapName);
	return true;
}

UStaticMesh* UMapExporter::NavMeshToStaticMeshV2(UWorld* World)
{
	if (!World) return nullptr;
	
	const ARecastNavMesh* NavData = nullptr;
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (NavSystem)
	{
		NavData = Cast<ARecastNavMesh>(NavSystem->GetDefaultNavDataInstance());
	}

	if (!NavData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[V2] NavSystem or NavData not found in world: %s. Attempting to find ARecastNavMesh actor directly."), *World->GetName());
		TArray<AActor*> NavMeshActors;
		UGameplayStatics::GetAllActorsOfClass(World, ARecastNavMesh::StaticClass(), NavMeshActors);
		if (NavMeshActors.Num() > 0)
		{
			NavData = Cast<ARecastNavMesh>(NavMeshActors[0]);
			UE_LOG(LogTemp, Log, TEXT("[V2] Successfully found ARecastNavMesh actor directly: %s"), *NavData->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[V2] Could not find ARecastNavMesh actor in world: %s"), *World->GetName());
			return nullptr;
		}
	}

	FRecastDebugGeometry Geometry;
	Geometry.bGatherPolyEdges = true;
	Geometry.bGatherNavMeshEdges = true;

	NavData->BeginBatchQuery();
	
	const int32 TileCount = NavData->GetNavMeshTilesCount();
	UE_LOG(LogTemp, Log, TEXT("[V2] World: %s, Tile Count: %d"), *World->GetName(), TileCount);

	int32 ValidTileCount = 0;
	for (int32 i = 0; i < TileCount; i++)
	{
		if (NavData->GetDebugGeometryForTile(Geometry, i))
		{
			ValidTileCount++;
		}
	}

	NavData->FinishBatchQuery();

	UE_LOG(LogTemp, Log, TEXT("[V2] Gathered geometry from %d tiles. Vertices: %d"), ValidTileCount, Geometry.MeshVerts.Num());

	if (Geometry.MeshVerts.Num() == 0) return nullptr;

	TArray<FVector> Vertices;
	TArray<FLinearColor> VertexColors;
	TArray<int32> Indices;

	auto* ProcMeshComp = NewObject<UProceduralMeshComponent>();

	for (int32 VertIdx = 0; VertIdx < Geometry.MeshVerts.Num(); ++VertIdx)
	{
		Vertices.Add(Geometry.MeshVerts[VertIdx]);
		VertexColors.Add(FLinearColor::Black); 
	}

	for (int32 AreaTypeIdx = 0; AreaTypeIdx < RECAST_MAX_AREAS; ++AreaTypeIdx)
	{
		auto& AreaIndices = Geometry.AreaIndices[AreaTypeIdx];
		for (int32 TriIdx = 0; TriIdx < AreaIndices.Num(); TriIdx += 3)
		{
			Indices.Add(AreaIndices[TriIdx]);
			Indices.Add(AreaIndices[TriIdx + 1]);
			Indices.Add(AreaIndices[TriIdx + 2]);
		}
	}

	if (Indices.Num() > 0)
	{
		ProcMeshComp->CreateMeshSection_LinearColor(0, Vertices, Indices, {}, {}, VertexColors, {}, false);
	}
	
	FMeshDescription MeshDescription = BuildMeshDescription(ProcMeshComp);
	if (MeshDescription.Polygons().Num() <= 0) return nullptr;
	
	UPackage* Package = GetTransientPackage();
	FName UniqueMeshName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), TEXT("TempNavMeshV2"));
	
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, UniqueMeshName, RF_Transient);
	StaticMesh->InitResources();
	StaticMesh->SetLightingGuid();
	if (StaticMesh->GetNumSourceModels() == 0) StaticMesh->AddSourceModel();
	
	FStaticMeshSourceModel& SrcModel = StaticMesh->GetSourceModel(0);
	SrcModel.BuildSettings.bRecomputeNormals = false;
	SrcModel.BuildSettings.bRecomputeTangents = false;
	SrcModel.BuildSettings.bGenerateLightmapUVs = false;

	StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
	StaticMesh->CommitMeshDescription(0);
	StaticMesh->SetBodySetup(nullptr);
	StaticMesh->Build(false);
	StaticMesh->PostEditChange();
	
	return StaticMesh;
}

bool UMapExporter::ExportMapDataJson(int32 MapId, const FString& MapName, UWorld* World, const TMap<FGuid, TPair<int32, int32>>& PortalGuidToInfoMap)
{
	if (!World) return false;
	
	TSharedPtr<FJsonObject> RootObj = MakeShared<FJsonObject>();
	RootObj->SetNumberField(TEXT("MapId"), MapId);
	RootObj->SetStringField(TEXT("MapName"), MapName);
	
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);
	TArray<TSharedPtr<FJsonValue>> PlayerStartJsonValues;
	for (const AActor* Actor : PlayerStarts)
	{
		if (!IsValid(Actor)) continue;
		
		TSharedPtr<FJsonObject> PlayerStartObj = MakeShared<FJsonObject>();

		const FVector Position = Actor->GetActorLocation();
		
		TSharedPtr<FJsonObject> PositionObj = MakeShared<FJsonObject>();
		PositionObj->SetNumberField(TEXT("X"), Position.X);
		PositionObj->SetNumberField(TEXT("Y"), Position.Y);
		PositionObj->SetNumberField(TEXT("Z"), Position.Z);
		
		PlayerStartObj->SetObjectField(TEXT("Position"), PositionObj);
		
		PlayerStartJsonValues.Add(MakeShared<FJsonValueObject>(PlayerStartObj));
	}
	RootObj->SetArrayField(TEXT("PlayerStarts"), PlayerStartJsonValues);
	
	TArray<AActor*> FieldPortals;
	UGameplayStatics::GetAllActorsOfClass(World, AFieldPortal::StaticClass(), FieldPortals);
	TArray<TSharedPtr<FJsonValue>> FieldPortalJsonValues;
	for (int32 i = 0; i < FieldPortals.Num(); ++i)
	{
		AFieldPortal* Portal = Cast<AFieldPortal>(FieldPortals[i]);
		if (!IsValid(Portal)) continue;
		
		TSharedPtr<FJsonObject> PortalObj = MakeShared<FJsonObject>();

		const FVector Position = Portal->GetActorLocation();
		
		TSharedPtr<FJsonObject> PositionObj = MakeShared<FJsonObject>();
		PositionObj->SetNumberField(TEXT("X"), Position.X);
		PositionObj->SetNumberField(TEXT("Y"), Position.Y);
		PositionObj->SetNumberField(TEXT("Z"), Position.Z);
		
		PortalObj->SetObjectField(TEXT("Position"), PositionObj);

		if (Portal->PortalId != i)
		{
			UE_LOG(LogTemp, Warning, TEXT("PortalId mismatch on %s: Actor PortalId=%d, Loop index=%d. Using actor PortalId."), *Portal->GetName(), Portal->PortalId, i);
		}
		PortalObj->SetNumberField(TEXT("PortalId"), Portal->PortalId);

		// 캐싱된 맵에서 목적지 MapId 및 PortalId 찾기
		if (const TPair<int32, int32>* TargetInfo = PortalGuidToInfoMap.Find(Portal->TargetGuid))
		{
			PortalObj->SetNumberField(TEXT("TargetMapId"), TargetInfo->Key);
			PortalObj->SetNumberField(TEXT("TargetPortalId"), TargetInfo->Value);
		}
		else
		{
			PortalObj->SetNumberField(TEXT("TargetMapId"), -1);
			PortalObj->SetNumberField(TEXT("TargetPortalId"), -1);
		}
		
		FieldPortalJsonValues.Add(MakeShared<FJsonValueObject>(PortalObj));
	}
	RootObj->SetArrayField(TEXT("FieldPortals"), FieldPortalJsonValues);
	
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (!FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer))
	{
		UE_LOG(MP2Editor, Error, TEXT("JSON Serialization failed"));
		return false;
	}
	
	FString OutPath = FPaths::ProjectDir() / TEXT("RecastCLI") / TEXT("Fields") / (MapName + TEXT(".json"));
	return FFileHelper::SaveStringToFile(OutputString, *OutPath);
}

bool UMapExporter::ExportNavMeshBin(const FString& MapName, UWorld* World)
{
	UStaticMesh* StaticMesh = NavMeshToStaticMesh(World);
	if (!StaticMesh) return false;
	ExportStaticMeshToBin(StaticMesh, MapName);
	return true;
}

UStaticMesh* UMapExporter::NavMeshToStaticMesh(UWorld* World)
{
	if (!World) return nullptr;
	
	const UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSystem) return nullptr;

	const ARecastNavMesh* NavData = Cast<ARecastNavMesh>(NavSystem->GetDefaultNavDataInstance());
	if (!NavData) return nullptr;

	// 추출할 데이터의 속성을 정의하는 구조체
	FRecastDebugGeometry Geometry;
	Geometry.bGatherPolyEdges = true;
	Geometry.bGatherNavMeshEdges = true;

	NavData->BeginBatchQuery();
	
	NavData->GetDebugGeometryForTile(Geometry, FNavTileRef()); 

	NavData->FinishBatchQuery();
	
	TArray<FVector> Vertices;
	TArray<FLinearColor> VertexColors;
	TArray<int32> Indices;

	auto* ProcMeshComp = NewObject<UProceduralMeshComponent>();

	// 내비게이션 영역(Area) 타입별로 루프를 돌면서 메쉬 섹션을 생성
	for (int32 VertIdx = 0; VertIdx < Geometry.MeshVerts.Num(); ++VertIdx)
	{
		Vertices.Add(Geometry.MeshVerts[VertIdx]);
		VertexColors.Add(FLinearColor::Black); 
	}

	// 모든 Area의 인덱스를 하나의 배열에 통폐합
	for (int32 AreaTypeIdx = 0; AreaTypeIdx < RECAST_MAX_AREAS; ++AreaTypeIdx)
	{
		auto& AreaIndices = Geometry.AreaIndices[AreaTypeIdx];
		for (int32 TriIdx = 0; TriIdx < AreaIndices.Num(); TriIdx += 3)
		{
			Indices.Add(AreaIndices[TriIdx]);
			Indices.Add(AreaIndices[TriIdx + 1]);
			Indices.Add(AreaIndices[TriIdx + 2]);
		}
	}

	// 루프가 끝난 뒤 섹션은 딱 하나(0번)만 생성
	if (Indices.Num() > 0)
	{
		ProcMeshComp->CreateMeshSection_LinearColor(0, Vertices, Indices, {}, {}, VertexColors, {}, false);
	}
	
	// 1. Procedural Mesh를 FMeshDescription 포맷으로 변환
	FMeshDescription MeshDescription = BuildMeshDescription(ProcMeshComp);
	if (MeshDescription.Polygons().Num() <= 0) return nullptr;
	
	UPackage* Package = GetTransientPackage();
	FName UniqueMeshName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), TEXT("TempNavMesh"));
	
	// 3. UStaticMesh 인스턴스 할당 및 리소스 초기화
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, UniqueMeshName, RF_Transient);
	StaticMesh->InitResources();
	
	StaticMesh->SetLightingGuid();
	if (StaticMesh->GetNumSourceModels() == 0)
	{
		StaticMesh->AddSourceModel();
	}
	
	// 4. 소스 모델 추가 및 불필요한 빌드 세팅 오프 (최적화)
	FStaticMeshSourceModel& SrcModel = StaticMesh->GetSourceModel(0);
	SrcModel.BuildSettings.bRecomputeNormals = false; // 내비용이니까 노멀 계산 안함
	SrcModel.BuildSettings.bRecomputeTangents = false;
	SrcModel.BuildSettings.bGenerateLightmapUVs = false; // 라이트맵 UV도 필요 없음

	StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
	StaticMesh->CommitMeshDescription(0);

	// 5. 물리 엔진용 콜리전 셋업 비활성화 및 빌드
	StaticMesh->SetBodySetup(nullptr);
	StaticMesh->Build(false);
	StaticMesh->PostEditChange();
	
	return StaticMesh;
}

void UMapExporter::ExportStaticMeshToBin(UStaticMesh* StaticMesh, const FString& OutFileName)
{
	if (!StaticMesh) return;
	
	FString DirectoryPath = FPaths::ProjectDir() / TEXT("RecastCLI") / TEXT("Origin");
	FString OutPath = DirectoryPath / (OutFileName + TEXT(".fbx"));
	
	if (!IFileManager::Get().DirectoryExists(*DirectoryPath))
	{
		IFileManager::Get().MakeDirectory(*DirectoryPath);
	}
	
	UAssetExportTask* ExportTask = NewObject<UAssetExportTask>();
	ExportTask->Object = StaticMesh;
	ExportTask->Exporter = nullptr;
	ExportTask->Filename = OutPath;
	ExportTask->bReplaceIdentical = false;
	ExportTask->bPrompt = false;   
	ExportTask->bUseFileArchive = false;
	ExportTask->bWriteEmptyFiles = false;   
	ExportTask->bAutomated = true; 
	
	UExporter::RunAssetExportTask(ExportTask);
	
	ExecuteRecastCLI(OutPath, (OutFileName + TEXT(".bin")));
}

void UMapExporter::ExecuteRecastCLI(const FString& Path, const FString& OutFileName)
{
	FString RecastCLIPath = FPaths::ProjectDir() / TEXT("RecastCLI");
	FString RecastCLIEXEPath = RecastCLIPath / TEXT("RecastCLI.exe");
	FString RecastOutPath = RecastCLIPath / TEXT("Fields") / TEXT("Navmesh") / OutFileName;
	
	FString ProcessParams = FString::Printf(TEXT("\"%s\" \"%s\""), *Path, *RecastOutPath);
	TSharedPtr<FMonitoredProcess> RecastProcess = MakeShared<FMonitoredProcess>(RecastCLIEXEPath, ProcessParams, true, true);
	RecastProcess->OnOutput().BindLambda([](const FString& OutputMessage)
	{
		UE_LOG(LogTemp, Log, TEXT("[Recast CLI] %s"), *OutputMessage);
	});
	
	RecastProcess->OnCompleted().BindLambda([RecastOutPath, RecastProcess](int32 ReturnCode) mutable {
		if (ReturnCode == 0) // 정상 종료 (Exit Code 0)
		{
			UE_LOG(LogTemp, Log, TEXT("성공적으로 bin가 생성되었습니다: %s"), *RecastOutPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("bin 변환에 실패했습니다. 종료 코드(Return Code): %d"), ReturnCode);
		}
		RecastProcess.Reset();
	});
	
	RecastProcess->Launch();
}