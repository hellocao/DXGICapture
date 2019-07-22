/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2018 Live Networks, Inc.  All rights reserved.
// A RTSP server
// C++ header

#ifndef _RTSP_SERVER_HH
#define _RTSP_SERVER_HH

#include "GenericMediaServer.hh"

#ifndef _DIGEST_AUTHENTICATION_HH
#include "DigestAuthentication.hh"
#endif

class RTSPServer: public GenericMediaServer 
{
public:
	static RTSPServer* createNew(UsageEnvironment& env, Port ourPort = 554,
					UserAuthenticationDatabase* authDatabase = NULL,
					unsigned reclamationSeconds = 65);

	static Boolean lookupByName(UsageEnvironment& env, char const* name,
					RTSPServer*& resultServer);

	typedef void (responseHandlerForREGISTER)(RTSPServer* rtspServer, unsigned requestId, int resultCode, char* resultString);
	unsigned registerStream(ServerMediaSession* serverMediaSession,
				char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum,
				responseHandlerForREGISTER* responseHandler,
				char const* username = NULL, char const* password = NULL,
				Boolean receiveOurStreamViaTCP = False,
				char const* proxyURLSuffix = NULL);

	typedef void (responseHandlerForDEREGISTER)(RTSPServer* rtspServer, unsigned requestId, int resultCode, char* resultString);
	unsigned deregisterStream(ServerMediaSession* serverMediaSession,
				char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum,
				responseHandlerForDEREGISTER* responseHandler,
				char const* username = NULL, char const* password = NULL,
				char const* proxyURLSuffix = NULL);
  
	char* rtspURL(ServerMediaSession const* serverMediaSession, int clientSocket = -1) const; //使用完毕使用delete[]释放

	char* rtspURLPrefix(int clientSocket = -1) const; //使用完毕使用delete[]释放

	char* rtspInputURL(ServerMediaSession const* serverMediaSession);

	//设置服务端权限,返回旧权限对象，具有唯一性
	//"newDB"如果为空的话表示删除权限 
	UserAuthenticationDatabase* setAuthenticationDatabase(UserAuthenticationDatabase* newDB);

	void disableStreamingRTPOverTCP() 
	{
		fAllowStreamingRTPOverTCP = False;
	}

	Boolean setInputStreamOverUDP(Port inputPort);
	Port GetinputServerPort() const;
	int  GetInputSocket() const;

	virtual char const* allowedCommandNames();
	virtual Boolean specialClientAccessCheck(int clientSocket, struct sockaddr_in& clientAddr, char const* urlSuffix);
	virtual UserAuthenticationDatabase* getAuthenticationDatabaseForCommand(char const* cmdName);
	// a hook that allows subclassed servers to do server-specific access checking
	// on each client (e.g., based on client IP address), without using digest authentication.
	virtual Boolean specialClientUserAccessCheck(int clientSocket, struct sockaddr_in& clientAddr,
		char const* urlSuffix, char const *username);
	Boolean fAllowStreamingRTPOverTCP; // by default, True
protected:
	RTSPServer(UsageEnvironment& env,
		int ourSocket, Port ourPort,
			UserAuthenticationDatabase* authDatabase,
			unsigned reclamationSeconds);  // called only by createNew();
     
	virtual ~RTSPServer();

	virtual Boolean weImplementREGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
						char const* proxyURLSuffix, char*& responseStr);
	// used to implement "RTSPClientConnection::handleCmd_REGISTER()"
	// Note: "responseStr" is dynamically allocated (or NULL), and should be delete[]d after the call
	virtual void implementCmd_REGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
						char const* url, char const* urlSuffix, int socketToRemoteServer,
						Boolean deliverViaTCP, char const* proxyURLSuffix);
	// used to implement "RTSPClientConnection::handleCmd_REGISTER()"

	// another hook that allows subclassed servers to do server-specific access checking
	// - this time after normal digest authentication has already taken place (and would otherwise allow access).
	// (This test can only be used to further restrict access, not to grant additional access.)

	// If you subclass "RTSPClientConnection", then you must also redefine this virtual function in order
	// to create new objects of your subclass:
	virtual ClientConnection* createNewClientConnection(int clientSocket, struct sockaddr_in clientAddr);
	// If you subclass "RTSPClientSession", then you must also redefine this virtual function in order
	// to create new objects of your subclass:
	virtual ClientSession* createNewClientSession(u_int32_t sessionId);

private: 
	virtual Boolean isRTSPServer() const;
private:
	void noteTCPStreamingOnSocket(int socketNum, RTSPClientSession* clientSession, unsigned trackNum);
	void unnoteTCPStreamingOnSocket(int socketNum, RTSPClientSession* clientSession, unsigned trackNum);
	void stopTCPStreamingOnSocket(int socketNum,bool& bIsTcpStream);

private:
	HashTable* fTCPStreamingDatabase; // maps TCP socket numbers to ids of sessions that are streaming over it (RTP/RTCP-over-TCP)
	HashTable* fPendingRegisterOrDeregisterRequests;
	unsigned fRegisterOrDeregisterRequestCounter;
	UserAuthenticationDatabase* fAuthDB;
	Port m_inputPort;
	int m_inputSocket;
};


////////// A subclass of "RTSPServer" that implements the "REGISTER" command to set up proxying on the specified URL //////////

class RTSPServerWithREGISTERProxying: public RTSPServer 
{
public:
	static RTSPServerWithREGISTERProxying* createNew(UsageEnvironment& env, Port ourPort = 554,
							UserAuthenticationDatabase* authDatabase = NULL,
							UserAuthenticationDatabase* authDatabaseForREGISTER = NULL,
							unsigned reclamationSeconds = 65,
							Boolean streamRTPOverTCP = False,
							int verbosityLevelForProxying = 0,
							char const* backEndUsername = NULL,
							char const* backEndPassword = NULL);

protected:
	RTSPServerWithREGISTERProxying(UsageEnvironment& env, int ourSocket, Port ourPort,
				UserAuthenticationDatabase* authDatabase, UserAuthenticationDatabase* authDatabaseForREGISTER,
				unsigned reclamationSeconds,
				Boolean streamRTPOverTCP, int verbosityLevelForProxying,
				char const* backEndUsername, char const* backEndPassword);
	// called only by createNew();
	virtual ~RTSPServerWithREGISTERProxying();

protected:
	  virtual char const* allowedCommandNames();
	  virtual Boolean weImplementREGISTER(char const* cmd /*"REGISTER" or "DEREGISTER"*/,
						  char const* proxyURLSuffix, char*& responseStr);
	  virtual void implementCmd_REGISTER(char const* cmd /*"REGISTER" or "DEREGISTER"*/,
						 char const* url, char const* urlSuffix, int socketToRemoteServer,
						 Boolean deliverViaTCP, char const* proxyURLSuffix);
	  virtual UserAuthenticationDatabase* getAuthenticationDatabaseForCommand(char const* cmdName);

private:
	  Boolean fStreamRTPOverTCP;
	  int fVerbosityLevelForProxying;
	  unsigned fRegisteredProxyCounter;
	  char* fAllowedCommandNames;
	  UserAuthenticationDatabase* fAuthDBForREGISTER;
	  char* fBackEndUsername;
	  char* fBackEndPassword;
}; 

#endif
