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
#include "Items/FieldPortal.h"

bool UMapExporter::ExportMapDataJson(int32 FieldId, const FString& MapName)
{
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return false;
	
	TSharedPtr<FJsonObject> RootObj = MakeShared<FJsonObject>();
	RootObj->SetNumberField(TEXT("FieldId"), FieldId);
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
	RootObj->SetArrayField(TEXT("PlayerStart"), PlayerStartJsonValues);
	
	TArray<AActor*> FieldPortals;
	UGameplayStatics::GetAllActorsOfClass(World, AFieldPortal::StaticClass(), FieldPortals);
	TArray<TSharedPtr<FJsonValue>> FieldPortalJsonValues;
	for (const AActor* Actor : FieldPortals)
	{
		if (!IsValid(Actor)) continue;
		
		TSharedPtr<FJsonObject> PlayerStartObj = MakeShared<FJsonObject>();

		const FVector Position = Actor->GetActorLocation();
		
		TSharedPtr<FJsonObject> PositionObj = MakeShared<FJsonObject>();
		PositionObj->SetNumberField(TEXT("X"), Position.X);
		PositionObj->SetNumberField(TEXT("Y"), Position.Y);
		PositionObj->SetNumberField(TEXT("Z"), Position.Z);
		
		PlayerStartObj->SetObjectField(TEXT("Position"), PositionObj);
		
		FieldPortalJsonValues.Add(MakeShared<FJsonValueObject>(PlayerStartObj));
	}
	RootObj->SetArrayField(TEXT("FieldPortal"), FieldPortalJsonValues);
	
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (!FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer))
	{
		UE_LOG(MP2Editor, Error, TEXT("JSON Serialization failed"));
		return false;
	}
	
	FString OutPath = FPaths::ProjectDir() / TEXT("RecastCLI") / TEXT("Out") / (MapName + TEXT(".json"));
	return FFileHelper::SaveStringToFile(OutputString, *OutPath);
}

bool UMapExporter::ExportNavMeshBin(const FString& MapName)
{
	UStaticMesh* StaticMesh = NavMeshToStaticMesh();
	if (!StaticMesh) return false;
	ExportStaticMeshToBin(StaticMesh, MapName);
	return true;
}

UStaticMesh* UMapExporter::NavMeshToStaticMesh()
{
	UWorld* World = nullptr;
	
	if (GEditor)
	{
		World = GEditor->GetEditorWorldContext().World();
	}

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
	
	UAssetExportTask* ExportTask = NewObject<UAssetExportTask>();
	ExportTask->Object = StaticMesh;
	ExportTask->Exporter = nullptr;
	ExportTask->Filename = FPaths::ProjectSavedDir() / (OutFileName + TEXT(".fbx"));
	ExportTask->bReplaceIdentical = false;
	ExportTask->bPrompt = false;   
	ExportTask->bUseFileArchive = false;
	ExportTask->bWriteEmptyFiles = false;   
	ExportTask->bAutomated = true; 
	
	UExporter::RunAssetExportTask(ExportTask);
	
	FString BlenderExePath = TEXT("C:/Program Files (x86)/Steam/steamapps/common/Blender/blender.exe");
	FString PythonScriptPath = FPaths::ProjectDir() / TEXT("Scripts/fbx_to_obj.py");
	
	FString InputFbxPath = FPaths::ProjectSavedDir() / (OutFileName + TEXT(".fbx"));
	FString OutputObjPath = FPaths::ProjectSavedDir() / (OutFileName + TEXT(".obj"));
	
	FString ProcessParams = FString::Printf(TEXT("-b -P \"%s\" -- \"%s\" \"%s\""), 
	*PythonScriptPath, *InputFbxPath, *OutputObjPath);
	TSharedPtr<FMonitoredProcess> BlenderProcess = MakeShared<FMonitoredProcess>(BlenderExePath, ProcessParams, true, true);
	BlenderProcess->OnOutput().BindLambda([](FString OutputMessage){
		UE_LOG(LogTemp, Log, TEXT("[Blender Python] %s"), *OutputMessage);
	});
	
	BlenderProcess->OnCompleted().BindLambda([OutputObjPath, BlenderProcess, OutFileName](int32 ReturnCode) mutable{
		if (ReturnCode == 0) // 정상 종료 (Exit Code 0)
		{
			UE_LOG(LogTemp, Log, TEXT("성공적으로 OBJ가 생성되었습니다: %s"), *OutputObjPath);
			ExecuteRecastCLI(OutputObjPath, (OutFileName + TEXT(".bin")));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("OBJ 변환에 실패했습니다. 종료 코드(Return Code): %d"), ReturnCode);
		}
		
		BlenderProcess.Reset();
	});
	BlenderProcess->Launch();
}

void UMapExporter::ExecuteRecastCLI(const FString& Path, const FString& OutFileName)
{
	FString RecastCLIPath = FPaths::ProjectDir() / TEXT("RecastCLI");
	FString RecastCLIEXEPath = RecastCLIPath / TEXT("RecastCLI.exe");
	FString RecastOutPath = RecastCLIPath / TEXT("Out") / OutFileName;
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