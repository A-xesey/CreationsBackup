#include "stdafx.h"
#include "RCB_Cheat.h"
#include "RCB_Listener.h"
#include "PropManager_BackupProperties.h"
#include "BackupCreationsEditor.h"
#include "BackupCreationsCityMusic.h"
#include "BackupCreationsScenarioMode.h"

using namespace Resource;
RCB_Listener* listener;

void Initialize() {
	SetBackupProperties();
	CreateReadBackupSettings();

	CheatManager.AddCheat("backupcreations", new RCB_Cheat());
	listener = new RCB_Listener();
	MessageManager.AddListener(listener, 0x24ce123);

	BackupCreationsEditor::Initialize();
	BackupCreationsCityMusic::Initialize();
	BackupCreationsScenarioMode::Initialize();
}

void Dispose() {
	MessageManager.RemoveListener(listener, 0x24ce123);
	listener = nullptr;
	WriteReleaseBackupSettings();

	BackupCreationsEditor::Dispose();
	BackupCreationsCityMusic::Dispose();
	BackupCreationsScenarioMode::Dispose();
}

void AttachDetours() {
	BackupCreationsEditor::AttachDetours();
	BackupCreationsCityMusic::AttachDetours();
	BackupCreationsScenarioMode::AttachDetours();
	RCB_Listener::AttachDetours();
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

