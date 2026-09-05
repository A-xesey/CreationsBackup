#include "stdafx.h"
#include "RCB_Cheat.h"
#include "eBackupProperties.h"

RCB_Cheat::RCB_Cheat() {}
RCB_Cheat::~RCB_Cheat() {}

using namespace ArgScript;
using namespace App;
extern PropertyListPtr BackupSettings;

inline uint32_t GetPropertyName(const string& option) {
	if (option == "edt") return kBackupByStepEDT;
	if (option == "cmp") return kBackupByStepCMP;
	if (option == "adv") return kBackupByStepADV;
	if (option == "enable") return kBackupEnable;
	if (option == "path") return kBackupPath;
	if (option == "drop") return kEditorAllowXMLFileDrop;
	return id("error");
}

void RCB_Cheat::SetInt32Property(const string& option, const string& s_value) {
	int value = mpFormatParser->ParseInt(s_value.c_str()); if (value < 0) value *= -1;
	BackupSettings->SetProperty(GetPropertyName(option), &Property().SetValueInt32(value));
	ConsolePrintF("%s changed to %i", PropManager.GetNameFromID(GetPropertyName(option)), value);
}

void RCB_Cheat::SetBoolProperty(const string& option, const string& s_value) {
	bool value = mpFormatParser->ParseBool(s_value.c_str());
	BackupSettings->SetProperty(GetPropertyName(option), &Property().SetValueBool(value));
	ConsolePrintF("%s changed to %i", PropManager.GetNameFromID(GetPropertyName(option)), value);
}
void RCB_Cheat::SetString16Property(const string& option, string16& s_value) {
	if (s_value.rfind(u"\\") != s_value[s_value.size() - 1] || s_value.rfind(u"/") != s_value[s_value.size() - 1])
		s_value += s_value.find(u"\\") == string16::npos ? u"/" : u"\\";
	BackupSettings->SetProperty(GetPropertyName(option), &Property().SetValueString16(s_value));
	ConsolePrintF("%s changed to %ls", PropManager.GetNameFromID(GetPropertyName(option)), s_value.c_str());
}

void RCB_Cheat::ParseLine(const Line& line)
{
	if (BackupSettings) {
		size_t totalrange;
		Arguments args = line.GetArgumentsRange(&totalrange, 0, 12);
		if (totalrange > 1 && totalrange % 2 == 0) {
			string s_arg, v_arg;
			for (size_t range = 0; range < totalrange; range += 2) {
				s_arg.assign(args[range]); v_arg.assign(args[range + 1]);
				if (s_arg == "edt" || s_arg == "cmp" || s_arg == "adv") SetInt32Property(s_arg, v_arg);
				else if (s_arg == "enable" || s_arg == "drop") SetBoolProperty(s_arg, v_arg);
				else if (s_arg == "path") SetString16Property(s_arg, string16().assign_convert(v_arg));
			}
		}
		else if (totalrange % 2 != 0 && string(args[0]) != "reset") ConsolePrintF("one of the arguments has no value");
		else if (string(args[0]) == "reset") {
			for (size_t i = 0; i < 5; i++)
				BackupSettings->SetProperty(0x4261636A + i, PropManager.GetPropertyDefinition(0x4261636A + i));
			BackupSettings->SetProperty(kEditorAllowXMLFileDrop, PropManager.GetPropertyDefinition(kEditorAllowXMLFileDrop));
			ConsolePrintF("the properties was reset");
			goto ShowProperties;
		}
		else {
ShowProperties:
			bool enable, drop;
			int edt, cmp, adv;
			string16 path;
			ConsolePrintF("BackupSettings.prop:");
			Property::GetBool(BackupSettings.get(), kEditorAllowXMLFileDrop, drop);
			Property::GetString16(BackupSettings.get(), kBackupPath, path);
			Property::GetBool(BackupSettings.get(), kBackupEnable, enable);
			Property::GetInt32(BackupSettings.get(), kBackupByStepEDT, edt);
			Property::GetInt32(BackupSettings.get(), kBackupByStepCMP, cmp);
			Property::GetInt32(BackupSettings.get(), kBackupByStepADV, adv);
			ConsolePrintF("EditorAllowXMLFileDrop: %i\nBackupPath: %ls\nBackupEnable: %i\nBackupByStepEDT: %i\nBackupByStepCMP: %i\nBackupByStepADV: %i", drop, path.c_str(), enable, edt, cmp, adv);
		}
	}
	else ConsolePrintF("BackupSettings.prop doesn't exists in %ls\\Preferences\\ path", Resource::Paths::GetDirFromID(Resource::PathID::AppData));
}

const char* RCB_Cheat::GetDescription(DescriptionMode mode) const
{
	return "BackupCreations: cheat to setting creations backup. If you write the cheat without arguments, you will get current backup properties values\n\treset\t\t\tresets creations backup to original values\n\tpath\t\t\tsets path to backup creations\n\t-enable\t\t\tsets enable (1) or disable (0) backup creations\n\t-edt\t\t\t\tsets a value after how many changes to backup creations (1 - all)\n\t-adv\t\t\t\tsets a value after how many changes to backup adventures (1 - all)\n\t-drop\t\t\tsets enable (1) or disable (0) creations files importing";
}
