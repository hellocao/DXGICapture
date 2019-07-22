#pragma once

#ifndef _MEDIA_HH
#include "Media.hh"
#endif

#include "NetAddress.hh"

class RTSPClient;
class InputStreamSource;

class InputSession :public Medium
{
public:
	static InputSession* createNew(UsageEnvironment& env, 
		char const* streamName = NULL,
		char const* description = NULL);

	void startStream(const char* transportParamsStr,RTSPClient* rtspClient);

	void stopStream();

	InputStreamSource* GetInputStreamSource() const;
protected:
	InputSession(UsageEnvironment& env, char const* streamName, char const* description);
	virtual ~InputSession();

	virtual InputStreamSource* createNewInputStreamSource(UsageEnvironment& env, int socketNum, 
		InputStreamSource* ClientSourceSource = NULL, 
		netAddressBits address = 0, portNumBits port = 0, u_int32_t fOurSessionId = 0);
private:
	InputStreamSource* m_inputStreamSource;
	int m_ourSocket;
	bool m_isInputoverTcp;
};

