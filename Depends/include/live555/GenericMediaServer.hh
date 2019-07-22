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
// A generic media server class, used to implement a RTSP server, and any other server that uses
//  "ServerMediaSession" objects to describe media to be served.
// C++ header

#ifndef _GENERIC_MEDIA_SERVER_HH
#define _GENERIC_MEDIA_SERVER_HH

#ifndef _MEDIA_HH
#include "Media.hh"
#endif
#ifndef _SERVER_MEDIA_SESSION_HH
#include "ServerMediaSession.hh"
#endif

#include "RTSPClientSession.h"
#include "RTSPClientConnection.h"

class GenericMediaServer: public Medium 
{
public:
	void addServerMediaSession(ServerMediaSession* serverMediaSession);

	virtual ServerMediaSession*
	lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession = True);

	void removeServerMediaSession(ServerMediaSession* serverMediaSession);
	void removeServerMediaSession(char const* streamName);

	void closeAllClientSessionsForServerMediaSession(ServerMediaSession* serverMediaSession);
	void closeAllClientSessionsForServerMediaSession(char const* streamName);

	void deleteServerMediaSession(ServerMediaSession* serverMediaSession);
	void deleteServerMediaSession(char const* streamName);

	unsigned numClientSessions() const { return fClientSessions->numEntries(); }

	void removeClientConnection(int clientSocket);
	ClientSession* createNewClientSessionWithId();

	unsigned fReclamationSeconds;
protected:
	GenericMediaServer(UsageEnvironment& env, int ourSocket, Port ourPort,
			unsigned reclamationSeconds); //在reclamationSeconds秒内如果检测到客户端没有活动将会回收
	virtual ~GenericMediaServer();
	void cleanup(); // MUST be called in the destructor of any subclass of us

	static int setUpOurSocket(UsageEnvironment& env, Port& ourPort);
	static void incomingConnectionHandler(void*, int /*mask*/);
	void incomingConnectionHandler();
	void incomingConnectionHandlerOnSocket(int serverSocket);

protected:
	virtual ClientConnection* createNewClientConnection(int clientSocket, struct sockaddr_in clientAddr) = 0;
	virtual ClientSession* createNewClientSession(u_int32_t sessionId) = 0;
	
	ClientSession* lookupClientSession(u_int32_t sessionId);
	ClientSession* lookupClientSession(char const* sessionIdStr);

	void addClientConnection(ClientConnection* clientConnection);
protected:
	int fServerSocket;
	Port fServerPort;
	
private:
	friend class ServerMediaSessionIterator;
	HashTable* fServerMediaSessions; // maps 'stream name' strings to "ServerMediaSession" objects
	HashTable* fClientConnections; // the "ClientConnection" objects that we're using
	HashTable* fClientSessions; // maps 'session id' strings to "ClientSession" objects
};

// An iterator over our "ServerMediaSession" objects:
class ServerMediaSessionIterator
{
public:
	ServerMediaSessionIterator(GenericMediaServer& server);
	virtual ~ServerMediaSessionIterator();
	ServerMediaSession* next();
private:
	HashTable::Iterator* fOurIterator;
};

// A data structure used for optional user/password authentication:

class UserAuthenticationDatabase 
{
public:
	UserAuthenticationDatabase(char const* realm = NULL,
				Boolean passwordsAreMD5 = False);
	// If "passwordsAreMD5" is True, then each password stored into, or removed from,
	// the database is actually the value computed
	// by md5(<username>:<realm>:<actual-password>)
	virtual ~UserAuthenticationDatabase();

	virtual void addUserRecord(char const* username, char const* password);
	virtual void removeUserRecord(char const* username);

	virtual char const* lookupPassword(char const* username);
	// returns NULL if the user name was not present

	char const* realm() { return fRealm; }
	Boolean passwordsAreMD5() { return fPasswordsAreMD5; }

	protected:
	HashTable* fTable;
	char* fRealm;
	Boolean fPasswordsAreMD5;
};

#endif
