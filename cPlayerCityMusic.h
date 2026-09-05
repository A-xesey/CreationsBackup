#pragma once
#include <EASTL\vector.h>
#include <EASTL/bonus/ring_buffer.h>
#include <Spore/App/MessageListenerData.h>
#include "cResourceCityMusic.h"

#define cPlayerCityMusicPtr eastl::intrusive_ptr<Audio::cPlayerCityMusic>

namespace Audio
{
	enum eNoteState {
		kNoteStateSuggested = 0,
		kNoteStateInScale = 1,
		kNoteStateOutOfScale = 2
	};

	struct cPlayerCityMusicConfig {
		uint32_t mNumAmbiences;
		uint32_t mNumRhythms;
		uint32_t mNumInstruments;
		uint32_t mMaxAmbienceLayers;
		uint32_t mMinNotes;
		uint32_t mMaxNotes;
		float* mpNumNoteDistribution;
		float mfBeatIncrement;
		float mfMaxBeats;
		uint32_t mMouseWheelIncrement;
		uint32_t field_28;
		uint32_t field_2C;
		vector<int> mpBeatDistSizes;
		vector<float*> mppBeatDistributions;
		uint32_t mTonic;
		uint32_t mMinPitch;
		uint32_t mMaxPitch;
		uint32_t* mpDegreeToPitch;
		float* mppDegreeTransitions[7];
		uint32_t* mppDegreeSuggestions[7];
		uint32_t mpStateToColor[3];
		uint32_t mMaxUndos;
		uint32_t mMouseWheelUndoTimeMs;
		ResourceKey* mppIcons[3];
		PropertyListPtr mpConfig;
		PropertyListPtr mpBeatConfig;
	};

	class cPlayerCityMusic
		: public Object
		, public App::IMessageListener
		, public DefaultRefCounted
	{
	public:
		using DefaultRefCounted::AddRef;
		using DefaultRefCounted::Release;
		
		cPlayerCityMusic();

		bool ResetUndoBuffer();
		bool LoadData(cResourceCityMusic*);
		void PlayMelody(uint32_t);

	public:
		cPlayerCityMusicConfig mConfig;
		ring_buffer<cResourceCityMusic, vector<cResourceCityMusic>> mUndoBuffer;
		ring_buffer_iterator<cResourceCityMusic, cResourceCityMusic*, cResourceCityMusic&, vector<cResourceCityMusic>> mCurrentState;
		bool mbPaused;
		bool mbIsMelodyLooping;
		RandomNumberGenerator mRandom;
		uint32_t mSoundId;
		uint32_t mPatchInstance;
		uint32_t mActiveAmbiences;
		vector<uint32_t> mAmbienceLayers;
		vector<eNoteState> mNoteStates;
		vector<uint32_t> mNoteDegrees;
		float mfMelodyDuration;
		uint32_t mBeatDistributionId;
		uint32_t mActiveNoteId;
		uint32_t mTempo;
		Clock mDownbeatProgress;
		string16 mName;
		string16 mDescription;
		fixed_vector<float, 3> mVolumes;
		list<uint32_t> mAmbienceHistory;
		uint32_t mRhythm;
		uint32_t mInstrument;
		vector<uint32_t> mNotePitches;
		vector<float> mNoteDurations;
		App::MessageListenerData mHandler;
		App::MessageListenerData mHandler2;
	};
	ASSERT_SIZE(cPlayerCityMusic, 0x220);

	inline cPlayerCityMusic::cPlayerCityMusic()
		: mConfig()
		, mUndoBuffer{}
		, mCurrentState {}
		, mbPaused(false)
		, mbIsMelodyLooping(false)
		, mRandom(RandomNumberGenerator())
		, mSoundId(id("music_city_planner"))
		, mPatchInstance()
		, mActiveAmbiences()
		, mAmbienceLayers {}
		, mNoteStates {}
		, mNoteDegrees {}
		, mfMelodyDuration()
		, mBeatDistributionId()
		, mActiveNoteId()
		, mTempo(0)
		, mDownbeatProgress(Clock((Clock::Mode)1000, false))
		, mName()
		, mDescription()
		, mVolumes {}
		, mAmbienceHistory{}
		, mRhythm()
		, mInstrument()
		, mNotePitches {}
		, mNoteDurations {}
		, mHandler()
		, mHandler2()
	{
		PropManager.GetPropertyList(id("playercitymusic"), 0x21407EE, this->mConfig.mpConfig);
		PropManager.GetPropertyList(id("playercitymusicbeats"), 0x21407EE, this->mConfig.mpConfig);
		this->mUndoBuffer.resize(1);
		uint32_t message = 0xF62DEF;
		this->mHandler.mpMessageIDs = { &message };
		this->mHandler.mNumMessageIDs = 1;
	}

	inline bool cPlayerCityMusic::ResetUndoBuffer() {
		return CALL
		(
			Address(ModAPI::ChooseAddress(0xe86090, 0xeb10a0)),
			bool,
			Args(const cPlayerCityMusic*),
			Args(this)
		);
	}

	inline bool cPlayerCityMusic::LoadData(cResourceCityMusic* pResource) {
		return CALL
		(
			Address(ModAPI::ChooseAddress(0xe85b30, 0xeb0b40)),
			bool,
			Args(const cPlayerCityMusic*, cResourceCityMusic*),
			Args(this, pResource)
		);
	}

	inline void cPlayerCityMusic::PlayMelody(uint32_t param) {
		CALL
		(
			Address(ModAPI::ChooseAddress(0xe82020, 0xeace80)),
			void,
			Args(const cPlayerCityMusic*, uint32_t),
			Args(this, 0)
		);
	}
}
