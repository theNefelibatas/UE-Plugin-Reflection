/* Copyright Reflection Contributors 2024-2026 */

#include "Importers/Constructor/ImportJobApi.h"
#include "Importers/Constructor/ImportJob.h"
#include "Importers/Constructor/TypesHelper.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Log.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DArray.h"
#include "Factories/Texture2DArrayFactory.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ScopeExit.h"
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

bool UReflectionImportApi::BuildTexture2DArray(const FString& ArrayPath, const TArray<FString>& LayerTexturePaths) {
	/* Engine factory gates creation behind this console var (default 0); enable for
	 * the call and restore the original value on every exit path */
	IConsoleVariable* AllowCreation = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AllowTexture2DArrayCreation"));
	if (AllowCreation == nullptr) {
		UE_LOG(LogReflection, Warning, TEXT("Texture2DArray build: console var r.AllowTexture2DArrayCreation not found"));
		return false;
	}
	const int32 AllowCreationOriginal = AllowCreation->GetInt();
	AllowCreation->Set(1);
	ON_SCOPE_EXIT {
		AllowCreation->Set(AllowCreationOriginal);
	};

	UTexture2DArrayFactory* Factory = NewObject<UTexture2DArrayFactory>();
	for (const FString& LayerPath : LayerTexturePaths) {
		UTexture2D* Layer = LoadObject<UTexture2D>(nullptr, *LayerPath);
		if (Layer == nullptr) {
			UE_LOG(LogReflection, Warning, TEXT("Texture2DArray build: layer not found: %s"), *LayerPath);
			return false;
		}
		Factory->InitialTextures.Add(Layer);
	}

	FString PackageName, AssetName;
	if (!ArrayPath.Split(TEXT("/"), &PackageName, &AssetName, ESearchCase::IgnoreCase, ESearchDir::FromEnd) || AssetName.IsEmpty()) {
		UE_LOG(LogReflection, Warning, TEXT("Texture2DArray build: invalid array path: %s"), *ArrayPath);
		return false;
	}

	UPackage* Package = CreatePackage(*ArrayPath);
	if (Package == nullptr) {
		return false;
	}

	UTexture2DArray* Array = Cast<UTexture2DArray>(Factory->FactoryCreateNew(
		UTexture2DArray::StaticClass(), Package, FName(*AssetName), RF_Public | RF_Standalone, nullptr, GWarn));
	if (Array == nullptr) {
		UE_LOG(LogReflection, Warning, TEXT("Texture2DArray build: factory failed for %s"), *ArrayPath);
		return false;
	}

	Array->PostEditChange();
	Array->MarkPackageDirty();

	Package->SetDirtyFlag(true);
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.Error = GWarn;
	SaveArgs.SaveFlags = SAVE_NoError;
	UPackage::SavePackage(Package, nullptr, *PackageFileName, SaveArgs);
	FAssetRegistryModule::AssetCreated(Array);

	return true;
}
