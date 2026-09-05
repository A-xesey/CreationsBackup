#pragma once
#include <Spore\BasicIncludes.h>
#include "CreateDirectory.h"
#include "FilePath.h"

extern string16 fpath;
class BackupCreationsEditor {
public:
	static void Initialize() {};
	static void Dispose() {};
	static void AttachDetours();
};

using namespace Editors;
static_detour(IsValid, bool(bitset<128>, bitset<128>)) {
	bool detoured(bitset<128> validity, bitset<128> filter) {
		bool res = original_function(validity, filter);
		if (!res && !fpath.empty() && GetFileFormat(fpath) != u".bem" && Editor.IsActive()) {
			validity[kValidityInvalidName] = false;
			validity[kValidityBlankName] = false;
			fpath.clear(); fpath.resize(0);
			return original_function(validity, filter);
		}
		return res;
	}
};

member_detour(CommitEditHistory, cEditor, void(bool, EditorStateEditHistory*)) {
	void detoured(bool arg1, EditorStateEditHistory * pStateHistory) {
		original_function(this, arg1, pStateHistory);
		if (Editor.mEditHistory.size() > 1 &&
			Editor.mEditHistory.size() % BackupExistsAndEnabled(Editor.mSaveExtension) == 0) {
			cEditorResourcePtr backup = Editor.mEditHistory[Editor.mEditHistoryIndex - 1];
			if (backup) {
				string16 path, name(Editor.mpEditorModel->GetName());
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
						Args(IO::IStream*, cEditorResource*, uint32_t),
						Args(file.get(), backup.get(), backup->GetResourceKey().typeID)
					);
					file->Close();
				}
			}
		}
	}
};

void BackupCreationsEditor::AttachDetours() {
	CommitEditHistory::attach(GetAddress(cEditor, CommitEditHistory));
	IsValid::attach(Address(ModAPI::ChooseAddress(0x4ede60, 0x4f3d80)));
}
