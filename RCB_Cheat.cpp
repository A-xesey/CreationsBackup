#include "stdafx.h"
#include "RCB_Cheat.h"
#include <Spore/App/cPropManager.h>

RCB_Cheat::RCB_Cheat() {}
RCB_Cheat::~RCB_Cheat() {}

using namespace ArgScript;
using namespace App;

extern PropertyListPtr BackupSettings;

void RCB_Cheat::ParseLine(const Line& line)
{
	if (BackupSettings) {
		bool bEnable, bDrop;
		int iEDT, iADV;
		Arguments propEnable = line.GetOption("enable", 1);
		Arguments propEDT = line.GetOption("edt", 1);
		Arguments propADV = line.GetOption("adv", 1);
		Arguments drop = line.GetOption("drop", 1);
		if (propEnable) {
			bEnable = mpFormatParser->ParseBool(propEnable[0]);
			BackupSettings->SetProperty(id("BackupEnable"), &Property().SetValueBool(bEnable));
			ConsolePrintF("BackupEnable now is %i", bEnable);
		}
		if (propEDT) {
			iEDT = mpFormatParser->ParseInt(propEDT[0]); if (iEDT < 0) iEDT *= -1;
			BackupSettings->SetProperty(id("BackupByStepEDT"), &Property().SetValueInt32(iEDT));
			ConsolePrintF("BackupByStepEDT now is %i", iEDT);
		}
		if (propADV) {
			iADV = mpFormatParser->ParseInt(propADV[0]); if (iADV < 0) iADV *= -1;
			BackupSettings->SetProperty(id("BackupByStepADV"), &Property().SetValueInt32(iADV));
			ConsolePrintF("BackupByStepADV now is %i", iADV);
		}
		if (drop) {
			bDrop = mpFormatParser->ParseBool(drop[0]);
			BackupSettings->SetProperty(0x594A0FB, &Property().SetValueBool(bDrop));
			GetAppProperties()->SetProperty(0x594A0FB, &Property().SetValueBool(bDrop));
			ConsolePrintF("EditorAllowXMLFileDrop now is %i", bDrop);
		}
		bool show = line.HasFlag("show");
		if (show) {
			Property::GetBool(BackupSettings.get(), id("BackupEnable"), bEnable);
			Property::GetInt32(BackupSettings.get(), id("BackupByStepEDT"), iEDT);
			Property::GetInt32(BackupSettings.get(), id("BackupByStepADV"), iADV);
			Property::GetBool(BackupSettings.get(), 0x594A0FB, bDrop);
			ConsolePrintF("BackupEnable: %i\nBackupByStepEDT: %i\nBackupByStepADV: %i\nEditorAllowXMLFileDrop: %i", bEnable, iEDT, iADV, bDrop);
		}
		if (!propEnable && !propEDT && !propADV && !show && !drop) ConsolePrintF("unknown option or argument");
	}
	else ConsolePrintF("BackupSettings.prop doesn't exists in %ls\\Preferences", Resource::Paths::GetDirFromID(Resource::PathID::AppData));
}

const char* RCB_Cheat::GetDescription(DescriptionMode mode) const
{
	return "BackupCreations: cheat for creations backup setting\n\t\t-show\t\tshows current values of backup properties\n\t-enable\t\tsets enable (1) or disable (0) backup creations\n\t-edt\t\t\t\tsets a value after how many changes to backup creations (1 - all)\n\t-adv\t\t\t\tsets a value after how many changes to backup adventures (1 - all)\n\t-drop\t\t\tsets enable (1) or disable (0) creations files importing";
}
