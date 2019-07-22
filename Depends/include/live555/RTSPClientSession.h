#pragma once

//前向声明避免重复包含
class GenericMediaServer;
class ServerMediaSession;
class RTSPServer;
class ServerInputSession;
class UsageEnvironment;
class ServerMediaSubsession;
class RTSPClientConnection;
#ifndef _NET_COMMON_H
#include "NetCommon.h"
#endif
#ifndef _USAGE_ENVIRONMENT_HH
#include "UsageEnvironment.hh"
#endif

class ClientSession 
{
public:
	ClientSession(GenericMediaServer& ourServer, u_int32_t sessionId);
	virtual ~ClientSession();

	void noteLiveness();
	u_int32_t getOurSessionId() const;
protected:
	UsageEnvironment& envir();
	static void noteClientLiveness(ClientSession* clientSession);
	static void livenessTimeoutTask(ClientSession* clientSession);

protected:
	GenericMediaServer& fOurServer;
	u_int32_t fOurSessionId;
	ServerMediaSession* fOurServerMediaSession;
	TaskToken fLivenessCheckTask;
};


// The state of an individual client session (using one or more sequential TCP connections) handled by a RTSP server:
class RTSPClientSession : public ClientSession 
{
public:
	RTSPClientSession(RTSPServer& ourServer, u_int32_t sessionId);
	virtual ~RTSPClientSession();

	virtual void handleCmd_SETUP(RTSPClientConnection* ourClientConnection,
								char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr);
	virtual void handleCmd_withinSession(RTSPClientConnection* ourClientConnection,
		char const* cmdName,
		char const* urlPreSuffix, char const* urlSuffix,
		char const* fullRequestStr);
	void deleteStreamByTrack(unsigned trackNum);
	Boolean fIsMulticast, fStreamAfterSETUP;

protected:
	// Make the handler functions for each command virtual, to allow subclasses to redefine them:
	virtual void handleCmd_TEARDOWN(RTSPClientConnection* ourClientConnection, ServerMediaSubsession* subsession);
	virtual void handleCmd_PLAY(RTSPClientConnection* ourClientConnection, ServerMediaSubsession* subsession, char const* fullRequestStr);
	virtual void handleCmd_PAUSE(RTSPClientConnection* ourClientConnection, ServerMediaSubsession* subsession);
	virtual void handleCmd_GET_PARAMETER(RTSPClientConnection* ourClientConnection, ServerMediaSubsession* subsession, char const* fullRequestStr);
	virtual void handleCmd_SET_PARAMETER(RTSPClientConnection* ourClientConnection, ServerMediaSubsession* subsession, char const* fullRequestStr);
	virtual void handleCmd_INPUT(RTSPClientConnection* ourClientConnection, char const* fullRequestStr);

protected:
	void reclaimStreamStates();
	void deleteInputStream();
	Boolean isMulticast() const { return fIsMulticast; }

	// Shortcuts for setting up a RTSP response (prior to sending it):
	void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr);
	void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr, u_int32_t sessionId);
	void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr, char const* contentStr);
	void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr, u_int32_t sessionId, char const* contentStr);

protected:
	RTSPServer& fOurRTSPServer; 
	ServerInputSession* m_serverInputSession;
	unsigned char fTCPStreamIdCount;
	Boolean usesTCPTransport() const { return fTCPStreamIdCount > 0; }
	unsigned fNumStreamStates;
	struct streamState 
	{
		ServerMediaSubsession* subsession;
		int tcpSocketNum;
		void* streamToken;
	} *fStreamStates;
};