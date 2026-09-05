#include "stdafx.h"
#include "PropManager_BackupProperties.h"
#include "eBackupProperties.h"

using namespace App;
using namespace Resource;

PropertyListPtr BackupSettings;
void SetBackupProperties() {
	cPropManager* propMan = (cPropManager*)PropManager.Get();
	if (propMan) {
		//hash_map to read properties from the stream
		propMan->mPropertyIDsToNames.emplace(kBackupByStepEDT, "BackupByStepEDT");
		propMan->mPropertyIDsToNames.emplace(kBackupByStepADV, "BackupByStepADV");
		propMan->mPropertyIDsToNames.emplace(kBackupByStepCMP, "BackupByStepCMP");
		propMan->mPropertyIDsToNames.emplace(kBackupEnable, "BackupEnable");
		propMan->mPropertyIDsToNames.emplace(kBackupPath, "BackupPath");

		//hash_map to wrtie properties to the stream
		propMan->mNamesToPropertyIDs.emplace("backupbystepedt", kBackupByStepEDT);
		propMan->mNamesToPropertyIDs.emplace("backupbystepadv", kBackupByStepADV);
		propMan->mNamesToPropertyIDs.emplace("backupbystepcmp", kBackupByStepCMP);
		propMan->mNamesToPropertyIDs.emplace("backupenable", kBackupEnable);
		propMan->mNamesToPropertyIDs.emplace("backuppath", kBackupPath);

		//hasm_map for using properties in the game
		propMan->mPropertyDefinitions.SetProperty(kBackupByStepEDT, &Property().SetValueInt32(4));
		propMan->mPropertyDefinitions.SetProperty(kBackupByStepADV, &Property().SetValueInt32(6));
		propMan->mPropertyDefinitions.SetProperty(kBackupByStepCMP, &Property().SetValueInt32(3));
		propMan->mPropertyDefinitions.SetProperty(kBackupEnable, &Property().SetValueBool(true));
		propMan->mPropertyDefinitions.SetProperty(kBackupPath, &Property().SetValueString16(Paths::GetDirFromID(PathID::AppData)));

		if (propMan->mPropertyIDsToNames.find(kEditorAllowXMLFileDrop) == propMan->mPropertyIDsToNames.end())
			propMan->mPropertyIDsToNames.emplace(kEditorAllowXMLFileDrop, "EditorAllowXMLFileDrop");

		if (propMan->mNamesToPropertyIDs.find("editorallowxmlfiledrop") == propMan->mNamesToPropertyIDs.end())
			propMan->mNamesToPropertyIDs.emplace("editorallowxmlfiledrop", kEditorAllowXMLFileDrop);

		propMan = nullptr;
	}
}

void CreateReadBackupSettings(){
	bool created = false;
	ResourceKey key = { id("BackupSettings"), TypeIDs::prop, 0x11ac192 };
	if (!ResourceManager.FindDatabase(key)) {
		BackupSettings = new PropertyList();
		BackupSettings->SetResourceKey(key);
		for (size_t i = 0; i < 5; i++) BackupSettings->SetProperty(0x4261636A + i, PropManager.GetPropertyDefinition(0x4261636A + i));
		/*BackupSettings->SetProperty(kBackupByStepEDT, PropManager.GetPropertyDefinition(kBackupByStepEDT));
		BackupSettings->SetProperty(kBackupByStepADV, PropManager.GetPropertyDefinition(kBackupByStepADV));
		BackupSettings->SetProperty(kBackupByStepCMP, PropManager.GetPropertyDefinition(kBackupByStepCMP));
		BackupSettings->SetProperty(kBackupEnable, PropManager.GetPropertyDefinition(kBackupEnable));
		BackupSettings->SetProperty(kBackupPath, PropManager.GetPropertyDefinition(kBackupPath));*/
		BackupSettings->SetProperty(kEditorAllowXMLFileDrop, &Property().SetValueBool(true));	//EditorAllowXMLFileDrop
		SaveNamedResource(BackupSettings.get(), u"BackupSettings.prop", Paths::GetSaveArea(SaveAreaID::Preferences));
		PropManager.AddPropertyList(BackupSettings.get(), key.instanceID, key.groupID);
		created = true;
	}
	PropManager.GetPropertyList(key.instanceID, key.groupID, BackupSettings);

	//i feel dumb. Properties check
	if (!created) {
		for (size_t i = 0; i < 5; i++)
			if (!BackupSettings->HasProperty(0x4261636A + i))
				BackupSettings->SetProperty(0x4261636A + i, PropManager.GetPropertyDefinition(0x4261636A + i));
		/*if (!BackupSettings->HasProperty(kBackupByStepEDT)) BackupSettings->SetProperty(kBackupByStepEDT, PropManager.GetPropertyDefinition(kBackupByStepEDT));
		if (!BackupSettings->HasProperty(kBackupByStepADV)) BackupSettings->SetProperty(kBackupByStepADV, PropManager.GetPropertyDefinition(kBackupByStepADV));
		if (!BackupSettings->HasProperty(kBackupByStepCMP)) BackupSettings->SetProperty(kBackupByStepCMP, PropManager.GetPropertyDefinition(kBackupByStepCMP));
		if (!BackupSettings->HasProperty(kBackupEnable)) BackupSettings->SetProperty(kBackupEnable, PropManager.GetPropertyDefinition(kBackupEnable));
		if (!BackupSettings->HasProperty(kBackupPath)) BackupSettings->SetProperty(kBackupPath, PropManager.GetPropertyDefinition(kBackupPath));*/
		if (!BackupSettings->HasProperty(kEditorAllowXMLFileDrop)) BackupSettings->SetProperty(kEditorAllowXMLFileDrop, PropManager.GetPropertyDefinition(kEditorAllowXMLFileDrop));
	}
	bool EditorAllowXMLFileDrop;
	Property::GetBool(BackupSettings.get(), kEditorAllowXMLFileDrop, EditorAllowXMLFileDrop);
	GetAppProperties()->SetProperty(kEditorAllowXMLFileDrop, &Property().SetValueBool(EditorAllowXMLFileDrop));
}

void WriteReleaseBackupSettings() {
	SaveNamedResource(BackupSettings.get(), u"BackupSettings.prop", Paths::GetSaveArea(SaveAreaID::Preferences));
	BackupSettings = nullptr;
}