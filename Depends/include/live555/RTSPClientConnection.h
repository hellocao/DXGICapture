#pragma once

#include "RTSPClientSession.h"
#include "DigestAuthentication.hh"

#ifndef REQUEST_BUFFER_SIZE
#define REQUEST_BUFFER_SIZE 20000 // for incoming requests
#endif
#ifndef RESPONSE_BUFFER_SIZE
#define RESPONSE_BUFFER_SIZE 20000
#endif

class ClientConnection 
{
public:
	ClientConnection(GenericMediaServer& ourServer, int clientSocket, struct sockaddr_in clientAddr);
	virtual ~ClientConnection();

	int GetClientSocket() const;
	struct sockaddr_in GetClientAddr() const;
	unsigned char fResponseBuffer[RESPONSE_BUFFER_SIZE];
protected:
	UsageEnvironment& envir();
	void closeSockets();

	static void incomingRequestHandler(void*, int /*mask*/);
	void incomingRequestHandler();
	virtual void handleRequestBytes(int newBytesRead) = 0;
	void resetRequestBuffer();

protected:
	GenericMediaServer& fOurServer;
	int fOurSocket;
	struct sockaddr_in fClientAddr;
	unsigned char fRequestBuffer[REQUEST_BUFFER_SIZE];
	unsigned fRequestBytesAlreadySeen, fRequestBufferBytesLeft;
};

class RTSPClientConnection : public ClientConnection
{
public:
	RTSPClientConnection(RTSPServer& ourServer, int clientSocket, struct sockaddr_in clientAddr);
	virtual ~RTSPClientConnection();

	// Shortcuts for setting up a RTSP response (prior to sending it):
	void setRTSPResponse(char const* responseStr);
	void setRTSPResponse(char const* responseStr, u_int32_t sessionId);
	void setRTSPResponse(char const* responseStr, char const* contentStr);
	void setRTSPResponse(char const* responseStr, u_int32_t sessionId, char const* contentStr);

	//¿ì½Ý»Ø¸´
	virtual void handleCmd_bad();
	virtual void handleCmd_notSupported();
	virtual void handleCmd_notFound();
	virtual void handleCmd_sessionNotFound();
	virtual void handleCmd_unsupportedTransport();

	char const* fCurrentCSeq;

	static void handleAlternativeRequestByte(void*, u_int8_t requestByte);
protected: 
	virtual void handleRequestBytes(int newBytesRead);
	virtual void handleCmd_OPTIONS();
	virtual void handleCmd_GET_PARAMETER(char const* fullRequestStr); 
	virtual void handleCmd_SET_PARAMETER(char const* fullRequestStr); 
	virtual void handleCmd_DESCRIBE(char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr);
	//virtual void handleCmd_REGISTER(char const* cmd,
	//	char const* url, char const* urlSuffix, char const* fullRequestStr,
	//	Boolean reuseConnection, Boolean deliverViaTCP, char const* proxyURLSuffix);

private:
	void resetRequestBuffer();
	void closeSocketsRTSP();
	void handleAlternativeRequestByte1(u_int8_t requestByte);
	Boolean authenticationOK(char const* cmdName, char const* urlSuffix, char const* fullRequestStr);
	void changeClientInputSocket(int newSocketNum, unsigned char const* extraData, unsigned extraDataSize);

	RTSPServer& fOurRTSPServer; // same as ::fOurServer
	int& fClientInputSocket;	// aliased to ::fOurSocket
	int fClientOutputSocket;
	Boolean fIsActive;
	unsigned char* fLastCRLF;
	unsigned fRecursionCount;
	Authenticator fCurrentAuthenticator; // used if access control is needed
	char* fOurSessionCookie; // used for optional RTSP-over-HTTP tunneling
	unsigned fBase64RemainderCount; // used for optional RTSP-over-HTTP tunneling (possible values: 0,1,2,3)
	RTSPClientSession* m_clientSession;
};