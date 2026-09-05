#pragma once
#include <Spore\BasicIncludes.h>
#include "CreateDirectory.h"
#include "ReadWrite_cResourceCityMusic.h"
#include "cResourceCityMusic.h"
#include "cPlayerCityMusic.h"

class BackupCreationsCityMusic {
public:
	static void Initialize() {};
	static void Dispose() {};
	static void AttachDetours();
};

member_detour(cPlayerCityMusic_AddUndoState, Audio::cPlayerCityMusic, void()) {
	void detoured() {
		original_function(this);
		if (this->mUndoBuffer.begin() != this->mUndoBuffer.end()) {
			if (this->mUndoBuffer.size() % BackupExistsAndEnabled(TypeIDs::cmp) == 0) {
				string16 path, name(this->mName);
				if (this->mCurrentState.mContainerIterator->GetResourceKey().instanceID == 0)
					this->mCurrentState.mContainerIterator->SetResourceKey({ this->mCurrentState.mContainerIterator->GetResourceKey().instanceID, TypeIDs::cmp, GroupIDs::CityMusic });
				if (name.empty()) {
					if (GameModeManager.GetActiveModeID() == kGameCiv)
						name = GameNounManager.GetPlayerCivilization()->GetName();
					else if (GameModeManager.GetActiveModeID() == kGameSpace)
						name = GetPlayerEmpire()->mEmpireName;
				}
				CreateBackupFileDirectory(path, name.c_str(), this->mCurrentState.mContainerIterator);
				FileStreamPtr file = new IO::FileStream(path.c_str());
				if (file->Open(IO::AccessFlags::Write, IO::CD::CreateNew)) {
					Write_cResourceCityMusic(file.get(), this->mCurrentState.mContainerIterator);
					file->Close();
				}
			}
		}
	}
};

void BackupCreationsCityMusic::AttachDetours() {
	cPlayerCityMusic_AddUndoState::attach(Address(ModAPI::ChooseAddress(0xe86130, 0xeb1140)));
}
