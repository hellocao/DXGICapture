#pragma once

#ifndef _MEDIA_HH
#include "Media.hh"
#endif

#include "InputStreamSource.h"
#include <map>

using namespace std;


class ProxyInputStreamSource:public InputStreamSource
{
public:
	static ProxyInputStreamSource* createNew(UsageEnvironment& env, int socketNum, InputStreamSource* ClientSourceSource,
		netAddressBits address = 0, portNumBits port = 0, u_int32_t fOurSessionId = 0);

	void setClientInputSource(u_int32_t fOurSessionId, InputStreamSource* clientInputSource);
	void removeClientInputSource(u_int32_t fOurSessionId);
protected:
	ProxyInputStreamSource(UsageEnvironment& env, int socketNum, InputStreamSource* ClientSourceSource,
		netAddressBits address, portNumBits port, u_int32_t fOurSessionId);
	virtual ~ProxyInputStreamSource();

	virtual void inputStreamSourceHandler(unsigned char* buffer, int size);
private:
	InputStreamSource* m_clientInputSource;
	map<u_int32_t, InputStreamSource*> m_clientInputSourceMap;
};