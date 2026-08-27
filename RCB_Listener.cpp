#include "stdafx.h"
#include "RCB_Listener.h"
#include "cOSFileDropNotificationMessager.h"
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

extern string16 fpath;

// The method that receives the message. The first thing you should do is checking what ID sent this message...
bool RCB_Listener::HandleMessage(uint32_t messageID, void* message)
{
	if (GameModeManager.GetActiveModeID() == GameModeIDs::kScenarioMode && messageID == 0x24ce123) {
		cOSFileDropNotificationMessager* mes = (cOSFileDropNotificationMessager*)message;
		if (mes != nullptr && !mes->mFileStrs.empty()) {
			fpath.assign(mes->mFileStrs[0]);
			string16 fname = fpath.rfind(u"/") != string16::npos ? fpath.substr(fpath.rfind(u"/") + 1) : fpath.substr(fpath.rfind(u"\\") + 1);
			ResourceKey key;
			ResourceKey::Parse(key, fname.c_str());
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
	else if (Editor.IsActive() && messageID == 0x24ce123) {
		using namespace Editors;
		cOSFileDropNotificationMessager* mes = (cOSFileDropNotificationMessager*)message;
		if (mes != nullptr && !mes->mFileStrs.empty()) {
			fpath.assign(mes->mFileStrs[0]);
			string16 wbem = mes->mFileStrs[0].substr(mes->mFileStrs[0].rfind(u"."));
			if (wbem == u".bem") {
				FileStreamPtr bem = new IO::FileStream(mes->mFileStrs[0].c_str());
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
						/*if (Editor.mModelTypes[0] != modelType && Editor.mEditorName == id("CreatureEditorSmall")) {
							if (modelType == kPlantSmall) Editor.mEditorName = id("FloraEditorSmall");
							else if (modelType == kPlantMedium) Editor.mEditorName = id("FloraEditorMedium");
							else if (modelType == kPlantLarge) Editor.mEditorName = id("FloraEditorLarge");
						}*/
						if (editorID != Editor.mEditorName)	//SetCurrentConfig
							CALL(Address(ModAPI::ChooseAddress(0x572880, 0x579720)), void, Args(cEditor*, uint32_t, int, int, int), Args(&Editor, editorID, 0, 0, 0));

						IDGenerator.GenerateForGroup(model->mKey, Editor.mSaveExtension, ReturnGroupByType(Editor.mSaveExtension));
						wbem = mes->mFileStrs[0].rfind(u"/") != string16::npos ? mes->mFileStrs[0].substr(mes->mFileStrs[0].rfind(u"/") + 1) : mes->mFileStrs[0].substr(mes->mFileStrs[0].rfind(u"\\") + 1);
						Editor.SetName(wbem.c_str());
						model->SetName(wbem.c_str());
						Editor.SetEditorModel(model);
						Editor.field_4B0 = true;
						Editor.field_4B1 = false;
						//InitializeUndoList
						CALL(Address(ModAPI::ChooseAddress(0x57f330, 0x586690)), void, Args(cEditor*), Args(&Editor));
						model = nullptr;
					}
					bem->Close();
				}
			}
		}
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return true;
}
