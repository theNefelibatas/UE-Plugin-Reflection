/* Copyright Reflection Contributors 2024-2026 */

#include "Settings/Runtime.h"

#include "Misc/FileHelper.h"
#include "Engine/EngineUtilities.h"
#include "Utilities/JsonHelpers.h"
#include "Settings/SettingsAccess.h"

/* Define Global Struct */
FRRuntime GReflectionRuntime;

bool FRRuntime::IsOlderUE4Target() const {
	return MajorVersion == 4 && MinorVersion != -1 && MinorVersion < 14;
}

bool FRRuntime::IsUE5() const {
	return MajorVersion == 5;
}

bool FRRuntime::IsUE4() const {
	return MajorVersion == 4;
}

void FRRuntime::Update() {
	FString AppDataPath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA"));
	AppDataPath = FPaths::Combine(AppDataPath, TEXT("FModel/AppSettings.json"));

	if (FString FileContent; FFileHelper::LoadFileToString(FileContent, *AppDataPath)) {
		if (TSharedPtr<FJsonObject> JsonObject; DeserializeJSONObject(FileContent, JsonObject)) {
			ExportDirectory.Path = JsonObject->GetStringField(TEXT("PropertiesDirectory")).Replace(TEXT("\\"), TEXT("/"));
		}
	}

	GReflectionRuntime.bEnableToolbarToggling = GetPlugin(GReflectionInternalName.ToString()) != nullptr;

	/* The Cloud profile carries the project name package paths are spelled with; local
	 * imports never pick a profile, so fall back to the plugin setting */
	if (GReflectionRuntime.Profile.ProjectName.IsEmpty()) {
		GReflectionRuntime.Profile.ProjectName = GetSettings()->AssetSettings.ProjectName;
	}
}