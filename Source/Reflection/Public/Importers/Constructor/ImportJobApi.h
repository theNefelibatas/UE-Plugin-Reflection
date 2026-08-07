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
};
