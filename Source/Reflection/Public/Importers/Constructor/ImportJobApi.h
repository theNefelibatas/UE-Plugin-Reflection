/* Copyright Reflection Contributors 2024-2026 */

#pragma once

#include "CoreMinimal.h"
#include "ImportJobApi.generated.h"

/* Python/Blueprint automation entry point for the JSON import queue */
UCLASS()
class REFLECTION_API UReflectionImportApi : public UObject {
	GENERATED_BODY()
public:
	/* Enqueues JSON export files for import, processed in the given order */
	UFUNCTION(BlueprintCallable, Category = "Reflection")
	static bool EnqueueFiles(const TArray<FString>& Files);

	/* Queries whether this plugin can import a given export type. Source of truth is
	 * TypesHelper::CanImport — Python automation must query this instead of mirroring
	 * the importer registry */
	UFUNCTION(BlueprintCallable, Category = "Reflection")
	static bool CanImportType(const FString& Type);

	/* Attaches (slot index -> material asset path) to a skeletal mesh's material slots
	 * and saves the package. Python cannot write SkeletalMesh material slots (the
	 * material_interface field is read-only in the binding), so the import pipeline
	 * calls this after materials are imported. Paths must already be verified by the
	 * caller; missing slots/materials are skipped. */
	UFUNCTION(BlueprintCallable, Category = "Reflection")
	static bool AttachMeshMaterials(const FString& MeshPath, const TMap<int32, FString>& SlotMaterials);
};
