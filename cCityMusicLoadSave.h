#pragma once
#include "Spore/BasicIncludes.h"
#include "cPlayerCityMusic.h"

#define cCityMusicLoadSavePtr eastl::intrusive_ptr<Audio::cCityMusicLoadSave>

namespace Audio
{
	class cCityMusicLoadSave
        : public DefaultRefCounted
        , public Sporepedia::IShopperListener
        , public UTFWin::IWinProc
        , public App::IMessageListener
    {
	public:
        /* 14h */	cPlayerCityMusicPtr mpMusic;
        /* 18h */	int* mpViewer;
        /* 1Ch */	UILayoutPtr mpThumbnailLayout;
        /* 20h */	UILayoutPtr mpSaveDialogLayout;
        /* 24h */	bool mbAssetIsVisible;
        /* 25h */	bool mbWaitingForSaveDialog;
        /* 26h */	bool mbWaitingForThumbnail;
        /* 28h */	uint32_t mJobID;
        /* 2Ch */	Graphics::RenderTargetID mRectID;
	};
    ASSERT_SIZE(cCityMusicLoadSave, 0x34);
}