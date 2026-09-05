#pragma once
#include <Spore\Simulator\cBaseSimulatorInputStrategy.h>
#include <Spore\App\IMessageListener.h>
#include "cCommunityEditor.h"

namespace Simulator {
    class cCityInputStrategy
        : public cBaseSimulatorInputStrategy
        , public App::IMessageListener
        , public RefCountTemplate
    {
    public:
        char _padding_[56];
        cCommunityEditorPtr mpCommunityEditor;
        char _padding2_[196];
    };
    ASSERT_SIZE(cCityInputStrategy, 0x148);
}