#pragma once

#include <Spore\BasicIncludes.h>

#define RCB_ListenerPtr intrusive_ptr<RCB_Listener>

class RCB_Listener 
	: public App::IMessageListener
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("RCB_Listener");

	RCB_Listener();
	~RCB_Listener();

	int AddRef() override;
	int Release() override;

	static void AttachDetours();
	
	// This is the function you have to implement, called when a message you registered to is sent.
	bool HandleMessage(uint32_t messageID, void* message) override;
};
