#pragma once
#include <Spore\BasicIncludes.h>
#include <string>
#include <iostream>
#include "eBackupProperties.h"

extern PropertyListPtr BackupSettings;
inline uint32_t GetPropertyName(uint32_t type) {
	using namespace TypeIDs;
	switch (type) {
	case cll:
	case crt:
	case vcl:
	case ufo:
	case bld:
	case flr: return kBackupByStepEDT;
	case cmp: return kBackupByStepCMP;
	case adv: return kBackupByStepADV;
	default: return 0;
	}
}

int BackupExistsAndEnabled(uint32_t type) {
	if (BackupSettings) {
		bool BackupEnable;
		int BackupByStep;
		App::Property::GetBool(BackupSettings.get(), kBackupEnable, BackupEnable);
		App::Property::GetInt32(BackupSettings.get(), GetPropertyName(type), BackupByStep);
		if (BackupEnable && BackupByStep > 0) return BackupByStep;
	}
	return INT_MAX;
}

using namespace Resource;
const char16_t* GetExtensionByType(uint32_t type) {
	using namespace TypeIDs;
	switch (type) {
	case cll: return u".cll";
	case crt: return u".crt";
	case vcl: return u".vcl";
	case ufo: return u".ufo";
	case bld: return u".bld";
	case flr: return u".flr";
	case adv: return u".adv";
	case cmp: return u".cmp";
	default: return u".unknown";
	}
}

const char16_t* GetFolderNameByGroup(uint32_t type) {
	using namespace TypeIDs;
	switch (type) {
	case cll: return u"Cells/";
	case crt: return u"Creatures/";
	case vcl: return u"Vehicles/";
	case ufo: return u"UFOs/";
	case bld: return u"Buildings/";
	case flr: return u"Plants/";
	case adv: return u"Adventures/";
	case cmp: return u"CityMusics/";
	default: return u"Unknown/";
	}
}

int GetFileCount(string16 path) {
	if (!path.empty()) {
		int count = 0;
		WIN32_FIND_DATA a;
		path.append(u"*.*");
		HANDLE res = FindFirstFile((LPCWSTR)path.c_str(), &a);
		if (res == INVALID_HANDLE_VALUE) return 0;
		do {
			count++;
		} while (FindNextFile(res, &a));
		FindClose(res);
		return count;
	}
	return 0;
}

inline void CreateFolder(const wchar_t* path) {
	int i = _wmkdir(path);
}

template <typename T>
void CreateBackupFileDirectory(string16& path, const char16_t* name, const T& backup) {
	App::Property::GetString16(BackupSettings.get(), kBackupPath, path);
	path.append(u"Backups/");
	CreateFolder((const wchar_t*)path.c_str());
	path.append(GetFolderNameByGroup(backup->GetResourceKey().typeID));
	CreateFolder((const wchar_t*)path.c_str());
	path.append(name).append(u"/");
	CreateFolder((const wchar_t*)path.c_str());
	path.append(name).append(u"_backup");
	path.append((const char16_t*)std::to_wstring(GetFileCount(path)).c_str());
	path.append(GetExtensionByType(backup->GetResourceKey().typeID));
}

void ScenarioModeBackup() {
	cScenarioResourcePtr backup = ScenarioMode.GetResource();
	if (backup) {
		string16 path, name(ScenarioMode.GetData()->GetName());
		if (name.empty()) {
			ResourceKey key =
			{
					static_cast<uint32_t>(ScenarioMode.field_AC),
					static_cast<uint32_t>(ScenarioMode.field_B0),
					static_cast<uint32_t>(ScenarioMode.field_B4)
			};
			if (key.instanceID == 0) {
				IDGenerator.GenerateForGroup(key, TypeIDs::adv, GroupIDs::Adventures);
				ScenarioMode.field_AC = key.instanceID;
				ScenarioMode.field_B0 = key.typeID;
				ScenarioMode.field_B4 = key.groupID;
			}
			ResourceManager.GetNameFromKey(key, name);
			backup->SetResourceKey(key);
			name = name.substr(name.find(u"!") + 1, name.size() - name.rfind(u".") - 1);
		}
		CreateBackupFileDirectory(path, name.c_str(), backup.get());
		FileStreamPtr file = new IO::FileStream(path.c_str());
		if (file->Open(IO::AccessFlags::Write, IO::CD::CreateNew)) {
			backup->Write(file.get());
			file->Close();
		}
	}
}

