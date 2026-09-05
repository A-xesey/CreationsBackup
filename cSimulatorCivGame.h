#pragma once

#include <Spore\UTFWin\IWinProc.h>
#include <Spore\App\IMessageListener.h>
#include <Spore\App\IGameMode.h>
#include <Spore\Clock.h>
#include "cCityInputStrategy.h"

#define SimulatorCivGame (*Simulator::cSimulatorCivGame::Get())

namespace Simulator
{
	class cSimulatorCivGame
		: public App::IGameModeObject
		, public cGonzagoSimulator
		, public UTFWin::IWinProc
		, public App::IMessageListener
	{
	public:
		//TODO Apart from the IGameModeObject methods, it also has another virtual method

		/* 28h */	char padding[204];
		/* 28h */	cCityInputStrategy* mpInputStrategy;
		/* 28h */	char padding2[100];
	public:
		static cSimulatorCivGame* Get() { return STATIC_CALL_(Address(ModAPI::ChooseAddress(0x165aaf8, 0xcf7620)), cSimulatorCivGame*); };
	};
	ASSERT_SIZE(cSimulatorCivGame, 0x150);
}