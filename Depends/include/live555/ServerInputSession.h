#pragma once

#ifndef _MEDIA_HH
#include "Media.hh"
#endif

#include "GroupsockHelper.hh"
#include "InputStreamSource.h"

class ServerInputSession :public Medium
{
public:
	static ServerInputSession* createNew(UsageEnvironment& env,
		char const* streamName = NULL,
		char const* description = NULL);

	static Boolean lookupByName(UsageEnvironment& env, char const* mediumName, ServerInputSession*& resultSession);

	char const* streamName() const { return fStreamName; }

	virtual void getStreamParameters(int& UdpSocketNumSocketNum,
									netAddressBits& serverAddress, 
									Port& serverPort);
	void startStream(int socketNum, TaskScheduler::BackgroundHandlerProc* handler, void* clientdata, 
		InputStreamSource& ClientSourceSource, portNumBits port, u_int32_t fOurSessionId = 0);

	void stopStream();
protected:
	ServerInputSession(UsageEnvironment& env, char const* streamName,
						char const* description);

	virtual ~ServerInputSession();

	virtual InputStreamSource* createNewInputStreamSource(UsageEnvironment& env, InputStreamSource& ClientSourceSource, 
		int socketNum, netAddressBits address, portNumBits port, u_int32_t fOurSessionId);
	InputStreamSource* m_inputStreamSource;
	static InputStreamSource* m_udpInputStreamSource;
private: 
	virtual Boolean isServerInputSession() const;

private:
	char* fStreamName;
	char* fDescriptionString;
	bool m_isUdpSource;
};
