#include "stdafx.h"
#include "_LoadAdventure.h"

using namespace Simulator;
using namespace App;
bool _LoadAdventure(cScenarioData* _this, ResourceKey* key) {
	bool result = false;
	//GetResourceByKey
	bool succed = CALL
		(
			Address(ModAPI::ChooseAddress(0xf18b90, 0xf44a90)),
			bool,
			Args(cScenarioData*, ResourceKey*),
			Args(_this, key)
		);
	if (succed) {
			CALL
			(
				Address(0xf2b270),
				bool,
				Args(cScenarioResource*),
				Args(_this->mpResource.get())
			);
			_this->mHadMissingAssets = CALL
			(
				Address(0xf43dc0),
				bool,
				Args(cScenarioData*),
				Args(_this)
			);
			CALL
			(
				Address(0xf261b0),
				void,
				Args(cScenarioResource*),
				Args(_this->mpResource.get())
			);
			if (ScenarioMode.mMode == cScenarioMode::Mode::UnkMode)
				ScenarioMode.mMode = cScenarioMode::Mode::EditMode;
			bool scenarioCacheModels;
			Property::GetBool(GetAppProperties(), 0x50caab1, scenarioCacheModels);
			CALL
			(
				Address(0xf411a0),
				void,
				Args(cScenarioData*),
				Args(_this)
			);
			//UpdateMarkers
			CALL
			(
				Address(0xf45ad0),
				void,
				Args(cScenarioData*, cScenarioMode::Mode, int, bool, bool),
				Args(_this, ScenarioMode.mMode, -(scenarioCacheModels != false), true, true)
			);
			GameNounManager.UpdateModels();
			//UpdateMarkers
			CALL
			(
				Address(0xf45ad0),
				void,
				Args(cScenarioData*, cScenarioMode::Mode, int, bool, bool),
				Args(_this, ScenarioMode.mMode, -(scenarioCacheModels != false), false, false)
			);
			result = true;
			_this->mHasLoaded = true;
			MessageManager.MessageSend(0x72bdb11, nullptr);
			MessageManager.MessageSend(0x7e1e46d, nullptr);
			ScenarioMode.field_AC = key->instanceID;
			ScenarioMode.field_B0 = key->typeID;
			ScenarioMode.field_B4 = key->groupID;
			_this->mGenreHasChanged = false;
			_this->mMetadataHasChanged = false;
	}
	return result;
}