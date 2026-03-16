// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapExporter.generated.h"

class UMP2FieldManifestDataAsset;

/**
 * 
 */
UCLASS()
class MP2EDITOR_API UMapExporter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MapExport")
	static bool ExportAllMaps();
	
	static bool ExportMapDataJson(int32 MapId, const FString& MapName, UWorld* World, const TMap<FGuid, TPair<int32, int32>>& PortalGuidToInfoMap);
	static bool ExportNavMeshBin(const FString& MapName, UWorld* World);
	static bool ExportNavMeshBinV2(const FString& MapName, UWorld* World);
	
private:
	static UStaticMesh* NavMeshToStaticMesh(UWorld* World);
	static UStaticMesh* NavMeshToStaticMeshV2(UWorld* World);
	static void ExportStaticMeshToBin(UStaticMesh* StaticMesh, const FString& OutFileName);
	static void ExecuteRecastCLI(const FString& Path, const FString& OutFileName);
};
