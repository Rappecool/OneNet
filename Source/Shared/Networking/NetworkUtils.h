#pragma once
#include <memory>
#include <WinSock2.h>

#define DEFAULT_PACKET_SIZE = 512

namespace ONet
{
	enum class ePacketType
	{
		InitConnection,
		InitPacket,
		NewConnection,
		TickPacket,
		RestartPacket,
		COUNT
	};

	class NetworkService
	{
		public:
		inline static int SendData(const SOCKET aSocket, const char* aMessage, const int aMessageSize, const int aFlags = 0)
		{
			//TODO: Error check
			return send(aSocket, aMessage, aMessageSize, aFlags);
		}
		inline static int ReceiveData(const SOCKET aSocket, char* aBuffer, const int aBufferSize, const int aFlags = 0)
		{
			//TODO: Error check.
			return recv(aSocket, aBuffer, aBufferSize, aFlags);
		}
	};


	struct Packet
	{
		unsigned int packetType = 0;
		char data[512];

		void Serialize(char* aData) const
		{
			memcpy(aData, this, sizeof(Packet));
		}

		void DeSerialize(char* aData)
		{
			memcpy(this, aData, sizeof(Packet));
		}
	};
}