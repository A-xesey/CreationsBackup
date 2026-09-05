#pragma once
#include <Spore\BasicIncludes.h>
#include "CreateDirectory.h"

extern string16 fpath;
class BackupCreationsScenarioMode {
public:
	static void Initialize() {};
	static void Dispose() {};
	static void AttachDetours();
};

using namespace Simulator;
member_detour(cScenarioData_GetResourceByKey, cScenarioData, bool(ResourceKey*)) {
	bool detoured(ResourceKey * key) {
		bool result = original_function(this, key);
		if (!result && !fpath.empty()) {
			FileStreamPtr file = new IO::FileStream(fpath.c_str());
			if (file->Open()) {
				cScenarioResourcePtr res = new cScenarioResource();
				if (res->Read(file.get())) {
					res->SetResourceKey(*key);
					this->SetName(GetFileName(fpath).c_str());
					this->mpResource = res;
					result = true;
				}
				file->Close();
			}
		}
		fpath.clear(); fpath.resize(0);
		return result;
	}
};

member_detour(cScenarioData_CommitHistoryEntry, cScenarioData, void()) {
	void detoured() {
		original_function(this);
		if (ScenarioEditHistory.mEntries.size() % BackupExistsAndEnabled(TypeIDs::adv) == 0)
			ScenarioModeBackup();
	}
};
member_detour(cScenarioTerraformMode_CommitHistoryEntry, cScenarioTerraformMode, void()) {
	void detoured() {
		original_function(this);
		if (ScenarioEditHistory.mEntries.size() % BackupExistsAndEnabled(TypeIDs::adv) == 0)
			ScenarioModeBackup();
	}
};

void BackupCreationsScenarioMode::AttachDetours() {
	cScenarioData_GetResourceByKey::attach(Address(ModAPI::ChooseAddress(0xf18b90, 0xf44a90)));
	cScenarioData_CommitHistoryEntry::attach(GetAddress(cScenarioData, CommitHistoryEntry));
	cScenarioTerraformMode_CommitHistoryEntry::attach(GetAddress(cScenarioTerraformMode, CommitHistoryEntry));
}
