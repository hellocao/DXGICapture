#pragma once

#ifndef _MEDIA_HH
#include "Media.hh"
#endif

#include "InputSession.h"

class ProxyInputSession :public InputSession
{
public:
	static ProxyInputSession* createNew(UsageEnvironment& env);

protected:
	ProxyInputSession(UsageEnvironment& env, char const* streamName, char const* description);
	virtual ~ProxyInputSession();

	virtual InputStreamSource* createNewInputStreamSource(UsageEnvironment& env, int socketNum,
		InputStreamSource* ClientSourceSource = NULL,
		netAddressBits address = 0, portNumBits port = 0, u_int32_t fOurSessionId = 0);
};