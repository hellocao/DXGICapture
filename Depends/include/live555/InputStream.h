#pragma once

#ifndef _MEDIA_HH
#include "Media.hh"
#endif
#include "GroupsockHelper.hh"

class InputStream :public Medium {

public:
	typedef void (inputStreamHandler)(unsigned char*, int);
	static InputStream* createNew(UsageEnvironment& env, inputStreamHandler* inputHandler);

	Boolean StartReceiveInputStream(int sockNum, Boolean streamUsingTCP);
	void StopReceiveInputStream(int sockNum);
	void setServerRequestAlternativeByteHandler(TaskScheduler::BackgroundHandlerProc* handler, void* clientData);
protected:
	InputStream(UsageEnvironment& env,inputStreamHandler* inputHandler);
	virtual ~InputStream();
	static void ReadTcpInputHandler(void*, int /*mask*/);
	static void ReadUdpInputHandler(void*, int /*mask*/);
	void incomingInputHandlerTcp();
	void incomingInputHandlerUdp();
private:
	int m_fServerSocket;
	unsigned char* m_fReceiveBuffer;
	int m_fReceiveBufferSize;
	TaskScheduler::BackgroundHandlerProc* m_serverRequestAlternativeByteHandler;
	void* m_serverRequestAlternativeByteHandlerClientData;
	unsigned short m_fNextTCPReadSize;
	enum 
	{ 
		AWAITING_DOLLAR,
		AWAITING_SIZE1, 
		AWAITING_SIZE2, 
		AWAITING_PACKET_DATA 
	} fTCPReadingState;
};