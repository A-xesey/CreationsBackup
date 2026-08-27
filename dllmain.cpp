#include "stdafx.h"
#include "RCB_Cheat.h"
#include "RCB_Listener.h"
#include "CreateDirectory.h"
#include <Spore/App/cPropManager.h>

using namespace App;
using namespace Resource;
RCB_Listener* listener;
PropertyListPtr BackupSettings;

void Initialize() {
	cPropManager* propMan = (cPropManager*)PropManager.Get();
	if (propMan) {
		//hash_map to read properties from the stream
		propMan->mPropertyIDsToNames.emplace(id("BackupByStepEDT"), "BackupByStepEDT");
		propMan->mPropertyIDsToNames.emplace(id("BackupByStepADV"), "BackupByStepADV");
		propMan->mPropertyIDsToNames.emplace(id("BackupEnable"), "BackupEnable");

		//hash_map to wrtie properties to the stream
		propMan->mNamesToPropertyIDs.emplace("backupbystepedt", id("BackupByStepEDT"));
		propMan->mNamesToPropertyIDs.emplace("backupbystepadv", id("BackupByStepADV"));
		propMan->mNamesToPropertyIDs.emplace("backupenable", id("BackupEnable"));

		//hasm_map for using properties in the game
		propMan->mPropertyDefinitions.SetProperty(id("BackupByStepEDT"), &Property().SetValueInt32(4));
		propMan->mPropertyDefinitions.SetProperty(id("BackupByStepADV"), &Property().SetValueInt32(8));
		propMan->mPropertyDefinitions.SetProperty(id("BackupEnable"), &Property().SetValueBool(true));

		if (propMan->mPropertyIDsToNames.find(0x594A0FB) == propMan->mPropertyIDsToNames.end())
			propMan->mPropertyIDsToNames.emplace(0x594A0FB, "EditorAllowXMLFileDrop");

		if (propMan->mNamesToPropertyIDs.find("editorallowxmlfiledrop") == propMan->mNamesToPropertyIDs.end())
			propMan->mNamesToPropertyIDs.emplace("editorallowxmlfiledrop", 0x594A0FB);

		propMan = nullptr;
	}
	bool created = false;
	ResourceKey key = { id("BackupSettings"), TypeIDs::prop, 0x11ac192 };
	if (!ResourceManager.FindDatabase(key)) {
		BackupSettings = new PropertyList();
		BackupSettings->SetResourceKey(key);
		BackupSettings->SetProperty(id("BackupByStepEDT"), PropManager.GetPropertyDefinition(id("BackupByStepEDT")));
		BackupSettings->SetProperty(id("BackupByStepADV"), PropManager.GetPropertyDefinition(id("BackupByStepADV")));
		BackupSettings->SetProperty(id("BackupEnable"), PropManager.GetPropertyDefinition(id("BackupEnable")));
		BackupSettings->SetProperty(0x594A0FB, &Property().SetValueBool(true));	//EditorAllowXMLFileDrop
		SaveNamedResource(BackupSettings.get(), u"BackupSettings.prop", Paths::GetSaveArea(SaveAreaID::Preferences));
		PropManager.AddPropertyList(BackupSettings.get(), key.instanceID, key.groupID);
		created = true;
	}
	PropManager.GetPropertyList(key.instanceID, key.groupID, BackupSettings);

	//i feel dumb. Properties checks
	if (!created) {
		if (!BackupSettings->HasProperty(id("BackupByStepEDT"))) BackupSettings->SetProperty(id("BackupByStepEDT"), PropManager.GetPropertyDefinition(id("BackupByStepEDT")));
		if (!BackupSettings->HasProperty(id("BackupByStepADV"))) BackupSettings->SetProperty(id("BackupByStepADV"), PropManager.GetPropertyDefinition(id("BackupByStepADV")));
		if (!BackupSettings->HasProperty(id("BackupEnable"))) BackupSettings->SetProperty(id("BackupEnable"), PropManager.GetPropertyDefinition(id("BackupEnable")));
		if (!BackupSettings->HasProperty(0x594A0FB)) BackupSettings->SetProperty(0x594A0FB, PropManager.GetPropertyDefinition(0x594A0FB));
	}
	bool EditorAllowXMLFileDrop;
	Property::GetBool(BackupSettings.get(), 0x594A0FB, EditorAllowXMLFileDrop);
	GetAppProperties()->SetProperty(0x594A0FB, &Property().SetValueBool(EditorAllowXMLFileDrop));

	CheatManager.AddCheat("backupcreations", new RCB_Cheat());
	listener = new RCB_Listener();
	MessageManager.AddListener(listener, 0x24ce123);
}

void Dispose() {
	MessageManager.RemoveListener(listener, 0x24ce123);
	listener = nullptr;
	SaveNamedResource(BackupSettings.get(), u"BackupSettings.prop", Paths::GetSaveArea(Resource::SaveAreaID::Preferences));
	BackupSettings = nullptr;
}

string16 fpath(u".");
using namespace Simulator;
member_detour(cScenarioData_GetResourceByKey, cScenarioData, bool(ResourceKey*)) {
	bool detoured(ResourceKey* key) {
		bool result = original_function(this, key);
		if (!result && !fpath.empty()) {
			FileStreamPtr file = new IO::FileStream(fpath.c_str());
			if (file->Open()) {
				cScenarioResourcePtr res = new cScenarioResource();
				if (res->Read(file.get())) {
					res->SetResourceKey(*key);
					this->mpResource = res;
					result = true;
				}
				file->Close();
			}
		}
		return result;
	}
};

static_detour(IsValid, bool(bitset<128>, bitset<128>)) {
	bool detoured(bitset<128> validity, bitset<128> filter) {
		bool res = original_function(validity, filter);
		if (!res && !fpath.empty()) {
			validity[Editors::kValidityInvalidName] = false;
			validity[Editors::kValidityBlankName] = false;
			return original_function(validity, filter);
		}
		return res;
	}
};

member_detour(cScenarioData_CommitHistoryEntry, cScenarioData, void()) {
	void detoured() {
		original_function(this);
		if (BackupSettings) {
			bool BackupEnable;
			int BackupByStepADV;
			Property::GetBool(BackupSettings.get(), id("BackupEnable"), BackupEnable);
			Property::GetInt32(BackupSettings.get(), id("BackupByStepADV"), BackupByStepADV);
			if (BackupEnable && (ScenarioEditHistory.mEntries.size() % BackupByStepADV == 0))
				ScenarioModeBackup();
		}
	}
};
member_detour(cScenarioTerraformMode_CommitHistoryEntry, cScenarioTerraformMode, void()) {
	void detoured() {
		original_function(this);
		if (BackupSettings) {
			bool BackupEnable;
			int BackupByStepADV;
			Property::GetBool(BackupSettings.get(), id("BackupEnable"), BackupEnable);
			Property::GetInt32(BackupSettings.get(), id("BackupByStepADV"), BackupByStepADV);
			if (BackupEnable && (ScenarioEditHistory.mEntries.size() % BackupByStepADV == 0))
				ScenarioModeBackup();
		}
	}
};

using namespace Editors;
member_detour(CommitEditHistory, cEditor, void(bool, EditorStateEditHistory*)) {
	void detoured(bool arg1, EditorStateEditHistory * pStateHistory) {
		original_function(this, arg1, pStateHistory);
		if (BackupSettings && Editor.mEditHistory.size() != 1) {
			bool BackupEnable;
			int BackupByStepEDT;
			Property::GetBool(BackupSettings.get(), id("BackupEnable"), BackupEnable);
			Property::GetInt32(BackupSettings.get(), id("BackupByStepEDT"), BackupByStepEDT);
			if (BackupEnable && (Editor.mEditHistory.size() % BackupByStepEDT == 0)) {
				cEditorResourcePtr backup = Editor.mEditHistory[Editor.mEditHistoryIndex - 1];
				if (backup) {
					string16 path;
					string16 name;
					name.append(Editor.mpEditorModel->GetName());
					if (name.empty()) {
						ResourceManager.GetNameFromKey(Editor.mpEditorModel->mKey, name);
						name = name.substr(name.find(u"!") + 1, name.size() - name.rfind(u".") - 1);
					}
					CreateBackupFileDirectory(path, name.c_str(), backup.get());
					FileStreamPtr file = new IO::FileStream(path.c_str());
					if (file->Open(IO::AccessFlags::Write, IO::CD::CreateNew)) {
						//SP::cSPEditorResourceFactory::WriteResourceToStream
						STATIC_CALL
						(
							Address(ModAPI::ChooseAddress(0x4b73d0, 0x4bd190)),
							bool,
							Args(IO::IStream*, Editors::cEditorResource*, uint32_t),
							Args(file.get(), backup.get(), backup->GetResourceKey().typeID)
						);
						file->Close();
					}
				}
			}
		}
	}
};

void AttachDetours() {
	cScenarioData_GetResourceByKey::attach(Address(ModAPI::ChooseAddress(0xf18b90, 0xf44a90)));
	CommitEditHistory::attach(GetAddress(Editors::cEditor, CommitEditHistory));
	cScenarioData_CommitHistoryEntry::attach(GetAddress(cScenarioData, CommitHistoryEntry));
	cScenarioTerraformMode_CommitHistoryEntry::attach(GetAddress(cScenarioTerraformMode, CommitHistoryEntry));
	IsValid::attach(Address(ModAPI::ChooseAddress(0x4ede60,0x4f3d80)));
}


// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

