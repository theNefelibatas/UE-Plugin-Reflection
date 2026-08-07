/* Copyright Reflection Contributors 2024-2026 */

#include "Importers/Constructor/ImportJobApi.h"
#include "Importers/Constructor/ImportJob.h"
#include "Importers/Constructor/TypesHelper.h"

bool UReflectionImportApi::EnqueueFiles(const TArray<FString>& Files) {
	FImportJob::Enqueue(Files);
	return true;
}

bool UReflectionImportApi::CanImportType(const FString& Type) {
	return CanImport(Type);
}
