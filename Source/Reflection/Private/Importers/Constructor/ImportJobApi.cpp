/* Copyright Reflection Contributors 2024-2026 */

#include "Importers/Constructor/ImportJobApi.h"
#include "Importers/Constructor/ImportJob.h"
#include "Importers/Constructor/TypesHelper.h"

#include "Engine/SkeletalMesh.h"
#include "Engine/SkinnedAssetCommon.h"
#include "UObject/SavePackage.h"

bool UReflectionImportApi::EnqueueFiles(const TArray<FString>& Files) {
	FImportJob::Enqueue(Files);
	return true;
}

bool UReflectionImportApi::CanImportType(const FString& Type) {
	return CanImport(Type);
}

bool UReflectionImportApi::AttachMeshMaterials(const FString& MeshPath, const TMap<int32, FString>& SlotMaterials) {
	USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);
	if (Mesh == nullptr) {
		return false;
	}

	TArray<FSkeletalMaterial>& Materials = Mesh->GetMaterials();
	for (const auto& [SlotIndex, MaterialPath] : SlotMaterials) {
		if (!Materials.IsValidIndex(SlotIndex)) {
			continue;
		}

		UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
		if (Material == nullptr) {
			continue;
		}

		Materials[SlotIndex].MaterialInterface = Material;
	}

	Mesh->PostEditChange();
	Mesh->MarkPackageDirty();

	UPackage* Package = Mesh->GetOutermost();
	if (Package == nullptr) {
		return false;
	}

	Package->SetDirtyFlag(true);
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.Error = GWarn;
	SaveArgs.SaveFlags = SAVE_NoError;
	UPackage::SavePackage(Package, nullptr, *PackageFileName, SaveArgs);

	return true;
}
