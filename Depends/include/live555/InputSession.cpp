#include "InputSession.h"
#include "RTSPClient.hh"
#include "GroupsockHelper.hh"

InputSession* InputSession::createNew(UsageEnvironment& env,
										char const* streamName,
										char const* description)
{
	return new InputSession(env, streamName, description);
}

InputStreamSource* InputSession::GetInputStreamSource() const
{
	return m_inputStreamSource;
}

InputSession::InputSession(UsageEnvironment& env, 
	char const* streamName, char const* description) :Medium(env),
	m_inputStreamSource(NULL),
	m_ourSocket(-1),
	m_isInputoverTcp(true)
{

}

InputSession::~InputSession()
{
	Medium::close(m_inputStreamSource);
	if (!m_isInputoverTcp)
	{
		::closeSocket(m_ourSocket);
	}
}

void InputSession::startStream(const char* transportParamsStr, RTSPClient* rtspClient)
{
	char* serverAddressStr = NULL;
	portNumBits serverPortNum = 0;
	unsigned char rtpChannelId = -1, rtcpChannelId = -1;

	if (!RTSPClient::parseTransportParams(transportParamsStr, serverAddressStr, serverPortNum, rtpChannelId, rtcpChannelId))
	{
		if (serverPortNum == 0) //TCP
		{
			m_isInputoverTcp = true;
			m_ourSocket = rtspClient->GetClientConnectSocket();
			if (m_ourSocket == -1) return;
			makeSocketNoDelay(m_ourSocket, True);
			m_inputStreamSource = createNewInputStreamSource(envir(), m_ourSocket);

			SocketDescriptor* socketDescriptor = lookupSocketDescriptor(envir(), m_ourSocket, False);
			TaskScheduler::BackgroundHandlerProc* handler = (TaskScheduler::BackgroundHandlerProc*)&(SocketDescriptor::tcpReadHandler);
			m_inputStreamSource->setServerRequestAlternativeByteHandler(handler, socketDescriptor);
		}
	}
	else if (serverAddressStr != NULL) //UDP
	{
		m_isInputoverTcp = false;
		m_ourSocket = setupDatagramSocket(envir(), 0);
		if (m_ourSocket == -1) return;
		Port port = serverPortNum;
		m_inputStreamSource = createNewInputStreamSource(envir(), m_ourSocket, NULL, our_inet_addr(serverAddressStr), port.num());
	}
}

void InputSession::stopStream()
{

}

InputStreamSource* InputSession::createNewInputStreamSource(UsageEnvironment& env, int socketNum, 
	InputStreamSource* ClientSourceSource, netAddressBits address, portNumBits port, u_int32_t fOurSessionId)
{
	return InputStreamSource::createNew(env, socketNum, address, port, fOurSessionId);
}