#pragma once
#include <string>

//TODO: Don't include everything in header.

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
#include "..\Shared\Networking\NetworkUtils.h"
#include "..\Shared\Networking\IPEndpoint.h"
#include <atomic>

namespace ONet
{
	enum class eClientType
	{
		Server, //We are a client, and server. AUTHORATIVE.
		Autonomous, //We are a client, that takes input etc, on the player machine.
		Simulated, //We are the simulated client that is being played on the server.
		COUNT
	};

	class Client
	{
	public:
		Client(const unsigned int aPort = 0, const std::string& aAdress = "127.0.0.1", const std::string& aName = "None");
		bool Init();
		void Tick();

		inline const std::string& GetName() const { return myName; }
		inline const bool GetIsActive() const { return myIsActive; }
		inline void SetIsActive(const bool aIsActive) { myIsActive = aIsActive; }
		
		void ReceiveData();
		bool ShutDown();

	private:
		bool ConnectToServer();

		IPEndPoint myIPEndpoint;
		std::atomic<bool> myHasEstablishedConnection = false;
		std::atomic<bool> myHasReceived = false;
		eClientType myClientType = eClientType::Autonomous;
		std::string myName = "";
		unsigned short myID = 0;
		bool myIsActive = false;
		SOCKET mySocket = INVALID_SOCKET;
		sockaddr_in myServerAddress;
		char mySendBuffer[DEFAULT_PACKET_SIZE];
		char myReceiveBuffer[DEFAULT_PACKET_SIZE];
	};
}