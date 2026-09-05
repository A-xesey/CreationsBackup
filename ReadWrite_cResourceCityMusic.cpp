#include "stdafx.h"
#include "ReadWrite_cResourceCityMusic.h"
#include "cSimulatorCivGame.h"

cCommunityEditor* GetCommunityEditor(const GameModeIDs& mode) {
	switch (mode) {
	case kGameCiv: return SimulatorCivGame.mpInputStrategy->mpCommunityEditor.get();
	case kGameSpace: return (cCommunityEditor*)SimulatorSpaceGame.mpCommunityEditor;
	default: return nullptr;
	}
}

void Write_cResourceCityMusic(IO::IStream* s, cResourceCityMusic* cm) {
	uint32_t header = 0x20434d50;   //CMP
	uint32_t version = 1;
	IO::WriteUInt32(s, &header);
	IO::WriteUInt32(s, &version);

	IO::WriteUInt32(s, &cm->mRhythm);
	IO::WriteUInt32(s, &cm->mInstrument);

	size_t size = cm->mPitches.size();
	IO::WriteUInt32(s, &size);
	IO::WriteUInt32(s, cm->mPitches.data(), cm->mPitches.size());
	IO::WriteFloat(s, cm->mDurations.data(), size);

	size = cm->mAmbiences.size();
	IO::WriteUInt32(s, &size);
	IO::WriteUInt32(s, cm->mAmbiences.data(), size);

	size = cm->mVolumes.size();
	IO::WriteUInt32(s, &size);
	IO::WriteFloat(s, cm->mVolumes.data(), size);
}

void Read_cResourceCityMusic(IO::IStream* s, cResourceCityMusic* cm) {
    uint32_t header, version;
    IO::ReadUInt32(s, &header);
    IO::ReadUInt32(s, &version);
    if ((header == 0x20434d50) && (version == 1)) {
        IO::ReadUInt32(s, &cm->mRhythm);
        IO::ReadUInt32(s, &cm->mInstrument);
        uint32_t size;

        IO::ReadUInt32(s, &size);
        cm->mPitches.resize(size);
        IO::ReadUInt32(s, cm->mPitches.data(), size);

        cm->mDurations.resize(size);
        IO::ReadFloat(s, cm->mDurations.data(), size);

        IO::ReadUInt32(s, &size);
        cm->mAmbiences.resize(size);
        IO::ReadUInt32(s, cm->mAmbiences.data(), size);

        IO::ReadUInt32(s, &size);
        cm->mVolumes.resize(size);
        IO::ReadFloat(s, cm->mVolumes.data(), size);
    }
}
