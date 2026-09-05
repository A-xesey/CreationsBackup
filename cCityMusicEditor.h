#pragma once
#include <Spore\UTFWin\IWinProc.h>
#include <Spore\App\IMessageListener.h>
//#include "cCityMusicLoadSave.h"
#include "cPlayerCityMusic.h"

#define cCityMusicEditorPtr eastl::intrusive_ptr<Audio::cCityMusicEditor>

namespace Audio {
    class cCityMusicEditor
        : public UTFWin::IWinProc
        , public App::IMessageListener
        , public RefCountTemplate
    {
    public:
        cPlayerCityMusicConfig mConfig;
        cPlayerCityMusicPtr mPlayerMusic;
        UILayoutPtr mpLayout;
        char _padding1_[24];
        //cCityMusicLoadSavePtr cCityMusicLoadSave;
        int cCityMusicLoadSave;
        char _padding2_[24];
    };
    ASSERT_SIZE(cCityMusicEditor, 0x114);
}