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

		bool AcceptClient(const unsigned int aClientID);
		//int ReceiveData(const unsigned int aClientID, char* aRecvBuffer);

	private:
		//Socket to listen for new connections.
		std::atomic<bool> myHasReceived = false;
		SOCKET myListenSocket = -1;
		std::unordered_map<unsigned int, IPEndPoint> myClients;
		unsigned int myNrOfClients = 0;
		unsigned short myMaxNrOfClients = 10;
		bool myIsFull = false;
		bool myIsActive = true;
		unsigned int myID = 0;
		char myBuffer[DEFAULT_PACKET_SIZE];
		int myBufferSize = -1;

		std::queue<NetMessage> myReceivedMessages;

		const unsigned int myTicksPerSecond = 60;
		const float myClientTimeOut = 10.0f;
	};
}