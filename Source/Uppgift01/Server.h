#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <atomic>

#define MAX_PACKET_SIZE 512


#define _WINSOCK_DEPRECATED_NOWARNINGS
#define _CRT_SECURE_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include "..\Shared\Networking\IPEndpoint.h"
#include "..\Shared\Networking\NetworkUtils.h"
#include "..\Shared\Messages\ChatMessage.h"


//TOOD: Handle threads.

//Have on thread for receiving.
//One thread for sending.


namespace ONet
{
	class Server
	{
	public:
		static Server& GetInstance();
		Server(const unsigned short aMaxNrOfClients = 10);

		bool Init();
		void Tick();
		void ReceiveClientData();
		bool ShutDown();

		//Send to client funcs.
		bool SendToOne(const unsigned int aClientIndex);
		bool SendToAllButOne(const unsigned int aClientIndex);
		bool SendToAll(const IPEndPoint& aClientEndPoint);

		//TODO: ADD RPC.
	private:
		void HandleClientJoin(sockaddr_in& client, const ChatMessage& aMsg);
		void HandleClientLeave(sockaddr_in& client);

		std::unordered_map<unsigned int, IPEndPoint> myClients;
		std::unordered_map<unsigned int, std::string> myClientNames;
		//Stores all messages we receive.
		std::queue<NetMessage> myReceivedMessages;
		//Socket to listen for new connections.
		SOCKET myListenSocket = -1;
		std::atomic<bool> myHasReceived = false;
		char myBuffer[DEFAULT_PACKET_SIZE];
		bool myIsFull = false;
		bool myIsActive = true;
		unsigned int myID = 0;
		unsigned int myNrOfClients = 0;
		int myBufferSize = -1;
		unsigned short myMaxNrOfClients = 10;


		const unsigned int myTicksPerSecond = 60;
		const float myClientTimeOut = 10.0f;
	};
}