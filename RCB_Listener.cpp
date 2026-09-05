#include "stdafx.h"
#include "RCB_Listener.h"
#include "cOSFileDropNotificationMessager.h"
#include "cResourceCityMusic.h"
#include "ReadWrite_cResourceCityMusic.h"
#include "FilePath.h"
//#include "_LoadAdventure.h"

RCB_Listener::RCB_Listener() {}
RCB_Listener::~RCB_Listener() {}

// For internal use, do not modify.
int RCB_Listener::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int RCB_Listener::Release()
{
	return DefaultRefCounted::Release();
}

inline uint32_t ReturnGroupByType(const uint32_t& type) {
	using namespace TypeIDs;
	using namespace GroupIDs;
	switch (type) {
	case cll: return CellModels;
	case bld: return BuildingModels;
	case vcl: return VehicleModels;
	case ufo: return UFOModels;
	case flr: return FloraModels;
	default: return CreatureModels;
	}
}

inline bool CityMusicEditorExists(cCityMusicEditor* cmeditor) {
	if (cmeditor) {
		if (!cmeditor->mPlayerMusic) return false;
		if (!cmeditor->mpLayout) return false;
		return true;
	}
	return false;
}

string16 fpath;
using namespace Editors;
member_detour(HandleFileDrop, cEditor, void(const char16_t*)) {
	void detoured(const char16_t* pFilePath) {
		if (!Editor.mIsActive) return;

		fpath.assign(pFilePath);
		if (!fpath.empty() && GetFileFormat(fpath) == u".bem") {
			FileStreamPtr bem = new IO::FileStream(fpath.c_str());
			if (bem->Open()) {
				cEditorResourcePtr creation = new cEditorResource();
				bool res = STATIC_CALL	//ReadResourceFromStream
				(
					Address(ModAPI::ChooseAddress(0x4ba150, 0x4bcc10)),
					bool,
					Args(IO::IStream*, cEditorResource*, uint32_t),
					Args(bem.get(), creation.get(), TypeIDs::bem)
				);
				if (res) {
					EditorModel* model = new EditorModel(); model->Load(creation.get());
					uint32_t modelType = creation->mProperties.mModelType;
					uint32_t editorID = STATIC_CALL(Address(ModAPI::ChooseAddress(0x432f00, 0x4333e0)), uint32_t, Args(uint32_t), Args(modelType));
					if (modelType == id("plant")) editorID = id("FloraEditorSetup");
					if (editorID != Editor.mEditorName)	//SetCurrentConfig
						CALL(Address(ModAPI::ChooseAddress(0x572880, 0x579720)), void, Args(cEditor*, uint32_t, int, int, int), Args(&Editor, editorID, 0, 0, 0));
					IDGenerator.GenerateForGroup(model->mKey, Editor.mSaveExtension, ReturnGroupByType(Editor.mSaveExtension));
					Editor.field_4B0 = true; Editor.field_4B1 = false;
					//InitializeUndoList
					CALL(Address(ModAPI::ChooseAddress(0x57f330, 0x586690)), void, Args(cEditor*), Args(&Editor));
					model = nullptr;
				}
				bem->Close();
			}
			fpath.clear(); fpath.resize(0);
			return;
		}
		return original_function(this, pFilePath);
	}
};

void RCB_Listener::AttachDetours() {
	HandleFileDrop::attach(Address(ModAPI::ChooseAddress(0x582810, 0x589ce0)));
}

// The method that receives the message. The first thing you should do is checking what ID sent this message...
bool RCB_Listener::HandleMessage(uint32_t messageID, void* message)
{
	GameModeIDs mode = (GameModeIDs)GameModeManager.GetActiveModeID();
	if (mode == kScenarioMode && messageID == 0x24ce123) {
		cOSFileDropNotificationMessager* mes = (cOSFileDropNotificationMessager*)message;
		fpath.assign(mes->mFileStrs[0]);
		if (mes != nullptr && !fpath.empty() && GetFileFormat(fpath) == u".adv") {
			string16 fname = GetFileName(fpath);
			ResourceKey key; ResourceKey::Parse(key, fname.c_str());
			if (key.groupID != GroupIDs::Adventures) key.groupID = GroupIDs::Adventures;
			//_LoadAdventure(ScenarioMode.GetData(), &key);
			CALL
			(
				Address(ModAPI::ChooseAddress(0xf1a540, 0xf464d0)),
				bool,
				Args(Simulator::cScenarioData*, ResourceKey*),
				Args(ScenarioMode.GetData(), &key)
			);
			/*FileStreamPtr file = new IO::FileStream(fpath.c_str());
			if (file->Open()) {
				cScenarioResourcePtr res = new Simulator::cScenarioResource();
				if (res->Read(file.get())) {
					string16 fname = fpath.rfind(u"/") != string16::npos ? fpath.substr(fpath.rfind(u"/") + 1) : fpath.substr(fpath.rfind(u"\\") + 1);
					ResourceKey key;
					ResourceKey::Parse(key, fname.c_str());
					res->SetResourceKey(key);
					CALL
					(
						Address(ModAPI::ChooseAddress(0xf1a540, 0xf464d0)),
						bool,
						Args(Simulator::cScenarioData*, ResourceKey*),
						Args(ScenarioMode.GetData(), &key)
					);
				}
			}
			file->Close();*/
			/*FileStreamPtr file = new IO::FileStream(mes->mFileStrs[0].c_str());
			if (file->Open()) {
				cScenarioResourcePtr res = new Simulator::cScenarioResource();
				if (res->Read(file.get())) {
					ResourceKey key;
					IDGenerator.GenerateForGroup(key, TypeIDs::adv, GroupIDs::Adventures);
					res->SetResourceKey(key);
					ResourceManager.CacheResource(res.get(), true);
					CALL
					(
						Address(0xf464d0),
						bool,
						Args(Simulator::cScenarioData*, ResourceKey*),
						Args(ScenarioMode.mpData.get(), &key)
					);
				}
			}
			file->Close();*/
		}
		mes = nullptr;
	}
	else if ((mode == kGameCiv || mode == kGameSpace) && messageID == 0x24ce123) {
		cOSFileDropNotificationMessager* mes = (cOSFileDropNotificationMessager*)message;
		cCommunityEditorPtr communityEditor = GetCommunityEditor(mode);
		if (mes != nullptr && !mes->mFileStrs[0].empty() && communityEditor &&
			CityMusicEditorExists(communityEditor->mCityMusicEditor.get())) {
			IWindowPtr cmplanner = communityEditor->mCityMusicEditor->mpLayout->FindWindowByID(0x045958E0);
			if (GetFileFormat(mes->mFileStrs[0]) == u".cmp" && cmplanner && cmplanner->IsVisible()) {
				FileStreamPtr cmp = new IO::FileStream(mes->mFileStrs[0].c_str());
				if (cmp->Open()) {
					cPlayerCityMusicPtr cmplayer = communityEditor->mCityMusicEditor->mPlayerMusic;
					cResourceCityMusicPtr cmres = new Audio::cResourceCityMusic();
					Read_cResourceCityMusic(cmp.get(), cmres.get());
					if (cmplayer->mUndoBuffer.size() > 0) cmplayer->ResetUndoBuffer();
					cmplayer->mCurrentState.mpContainer->front() = *cmres.get();
					cmplayer->LoadData(cmres.get());
					cmplayer->PlayMelody(0);
					cmplayer->mName = GetFileName(mes->mFileStrs[0]);
					cmp->Close();
				}
			}
		}
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return true;
}
