#pragma once
#include <unordered_map>
#include <string>

#define MAX_PACKET_SIZE 512

typedef unsigned __int64        SOCKET;

//TOOD: Handle threads.

//Have on thread for receiving.
//One thread for sending.


namespace ONet
{
class Client;
	class Server
	{
	public:
		static Server& GetInstance();
		Server(const unsigned short aMaxNrOfClients = 10);

		bool Init();
		void Tick();
		void Temp();

		bool AcceptClient(const unsigned int aClientID);
		int ReceiveData(const unsigned int aClientID, char* aRecvBuffer);

	private:
		//Socket to listen for new connections.
		SOCKET myListenSocket = -1;
		//SOCKET to give to clients.
		SOCKET myClientSocket = -1;
		std::unordered_map<unsigned int, SOCKET> myClientIDs;
		unsigned int myNrOfClients = 0;
		unsigned short myMaxNrOfClients = 10;
		bool myIsFull = false;
		bool myIsActive = true;
		unsigned int myID = 0;

		const unsigned int myTicksPerSecond = 60;
		const float myClientTimeOut = 10.0f;
	};
}