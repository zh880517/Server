#pragma once
#include "NetInclude.h"
#include "../Source/SharePtrDef.h"

#include <memory>
#include <string>


class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
	TCPConnection(TCPConnectPool* connPool, Net::ip::tcp::socket& socket, size_t buffLen);
	~TCPConnection();

public: 

	void TryRecive();

	void Close();

	void SetPacketQueue(PacketQueue* queue);

	void TrySend();

	void SetSession(TCPSession* pSession);

	TCPSession* Session() { return m_pSession; }

	Net::ip::tcp::socket& Socket() { return m_Socket; }
protected:

	
	void ReadHead();

	void ReadBody();

	void Send();

private:
	Net::ip::tcp::socket m_Socket;
	TCPConnectPool*		 m_ConnPool;
	TCPSession*			 m_pSession = nullptr;
	

	bool			m_bReading = false;

	char*			m_pBuffer = nullptr;
	size_t			m_iBufferLen = 0;
	size_t			m_iReadBytes = 0;
	std::string*	m_pCurRecivePacket = nullptr;

	size_t			m_iWriteBytes = 0;
	NetPacket		m_pCurSendPacket = nullptr;
	PacketQueue*	m_Packets = nullptr;
	std::mutex		m_SendMutex;

};

