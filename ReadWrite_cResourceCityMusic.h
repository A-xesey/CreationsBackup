#pragma once
#include "stdafx.h"
#include "cResourceCityMusic.h"
#include "cCommunityEditor.h"

using namespace Simulator;
cCommunityEditor* GetCommunityEditor(const GameModeIDs& mode);

using namespace Audio;
void Write_cResourceCityMusic(IO::IStream* s, cResourceCityMusic* cm);
void Read_cResourceCityMusic(IO::IStream* s, cResourceCityMusic* cm);
