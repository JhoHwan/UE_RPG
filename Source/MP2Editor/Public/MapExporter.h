// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapExporter.generated.h"

/**
 * 
 */
UCLASS()
class MP2EDITOR_API UMapExporter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MapExport")
	static bool ExportMapDataJson(int32 FieldId, const FString& MapName);

	
	UFUNCTION(BlueprintCallable, Category = "MapExport")
	static bool ExportNavMeshBin(const FString& MapName);
	
private:
	static UStaticMesh* NavMeshToStaticMesh();
	static void ExportStaticMeshToBin(UStaticMesh* StaticMesh, const FString& OutFileName);
	static void ExecuteRecastCLI(const FString& Path, const FString& OutFileName);
};
