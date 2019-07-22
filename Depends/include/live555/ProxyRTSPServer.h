#pragma once

#ifndef _RTSP_SERVER_HH
#include "RTSPServer.hh"
#endif



class ProxyRTSPServer :public RTSPServer
{
public:
	static ProxyRTSPServer* createNew(UsageEnvironment& env, Port ourPort = 554,
		UserAuthenticationDatabase* authDatabase = NULL,
		unsigned reclamationSeconds = 65);

protected:
	ProxyRTSPServer(UsageEnvironment& env,
		int ourSocket, Port ourPort,
		UserAuthenticationDatabase* authDatabase,
		unsigned reclamationSeconds);
	// called only by createNew();
	virtual ~ProxyRTSPServer();

	virtual ClientSession* createNewClientSession(u_int32_t sessionId);
};


class ProxyRTSPClientSession : public RTSPClientSession
{
public:
	ProxyRTSPClientSession(RTSPServer& ourServer, u_int32_t sessionId);
	virtual ~ProxyRTSPClientSession();

	virtual void handleCmd_INPUT(RTSPClientConnection* ourClientConnection,
		char const* fullRequestStr);
};
