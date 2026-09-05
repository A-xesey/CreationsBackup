#pragma once
#include <Spore\App\IMessageListener.h>
#include <Spore\Sporepedia\IShopperListener.h>
#include <Spore\UTFWin\IWinProc.h>
#include <Spore\UTFWin\UILayout.h>
#include "cCityMusicEditor.h"

#define cCommunityEditorPtr eastl::intrusive_ptr<Simulator::cCommunityEditor>

namespace Simulator {
    class cCommunityEditor
        : public App::IMessageListener
        , public Sporepedia::IShopperListener
        , public UTFWin::IWinProc
        , public RefCountTemplate
    {
    public:
        using RefCountTemplate::AddRef;
        using RefCountTemplate::Release;
    public:
        char _padding_[116];
        UTFWin::UILayout mEditorUI;
        char _padding2_[12];
        cCityMusicEditorPtr mCityMusicEditor;
        char _padding3_[28];
        Vector3 mRotationAnchor;
        Quaternion mAnchorQuaternion;
        Vector3 mMouseOffset;
        char _padding4_[520];
    };
    ASSERT_SIZE(cCommunityEditor, 0x2fc);
}