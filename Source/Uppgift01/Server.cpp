#include "Server.h"
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
#include "..\Shared\Messages\ChatMessage.h"
#include "..\Shared\Messages\MessageUtils.h"


ONet::Server& ONet::Server::GetInstance()
{
	static Server instance(10);
	return instance;
}

ONet::Server::Server(const unsigned short aMaxNrOfClients)
	:myMaxNrOfClients(aMaxNrOfClients)
{
	myListenSocket = INVALID_SOCKET;
}

bool ONet::Server::Init()
{
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsSuccess = WSAStartup(version, &data);
	if (wsSuccess != 0)
	{
		assert(false && "Failed to init server_Winsock");
		return false;
	}

	//Create socket.
	myListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in serverHint;
	//Get any open address on network card.
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_family = AF_INET;

	//htons converts how bytes are stored in memory. Converts from decimals to hexadecimal for network byte order.
	//htons = host-to-network short.
	serverHint.sin_port = htons(54000); //Convert from little to big endian.

	//Bind socket.
	//Socket is bound to port/IP in one.
	if (bind(myListenSocket, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		printf("Can't bind socket! %d", WSAGetLastError());
		return false;
	}

	myBufferSize = sizeof(ChatMessage);

	ZeroMemory(&myBuffer, myBufferSize);

	return true;
}

void ONet::Server::Tick()
{
	if (!myIsActive || !myHasReceived)
	{
		return;
	}

	std::cout << "SERVER TICK" << std::endl;
	//std::cout << "Accepted packet: " << static_cast<unsigned int>(myBuffer[0]) <<  ": " << Utils::DeSerializeMessageString(myBuffer) << std::endl;

	for (unsigned int messageIndex = 0; messageIndex < myReceivedMessages.size(); messageIndex++)
	{
		for (unsigned int clientIndex = 0; clientIndex < myClients.size(); clientIndex++)
		{
			sockaddr_in client;
			int clientLength = sizeof(client);
			ZeroMemory(&client, clientLength);
			client.sin_family = AF_INET;
			client.sin_addr.S_un.S_addr = std::atoi(myClients[clientIndex].GetAdress().c_str());
			client.sin_port = myClients[clientIndex].GetPort();

			int bufferSize = sizeof(ChatMessage);

			int sendSuccess = sendto(myListenSocket, myBuffer, bufferSize, 0, (sockaddr*)&client, clientLength);
			if (sendSuccess == SOCKET_ERROR)
			{
				std::cout << "Failed to send client data: " << WSAGetLastError() << std::endl;
			}
			else
			{
				std::cout << "Successfully sent packet to: " << myClients[clientIndex].GetPort() << std::endl;
			}
		}

		myReceivedMessages.pop();
	}

	//Empty buffer after we've sent.
	ZeroMemory(&myBuffer, myBufferSize);
	myHasReceived = false;
}

void ONet::Server::ReceiveClientData()
{
	sockaddr_in client;
	int clientLength = sizeof(client);
	ZeroMemory(&client, clientLength);

	const int bufferSize = sizeof(ChatMessage);

	std::cout << "ReceiveData()" << std::endl;

	//ZeroMemory(&myBuffer, bufferSize);
	const int bytesIn = recvfrom(myListenSocket, myBuffer, bufferSize, 0, (sockaddr*)&client, &clientLength);

	if (bytesIn <= 0)
	{
		return;
	}

	if (bytesIn == SOCKET_ERROR)
	{
		std::cout << "Error receiving from client: " << WSAGetLastError() << std::endl;
	}

	else if (static_cast<eMessageType>(myBuffer[0]) == eMessageType::CLIENT_Join)
	{
		std::cout << "Found new client! ID: " << myNrOfClients << std::endl;
		IPEndPoint fromClient(std::to_string(client.sin_addr.S_un.S_addr), client.sin_port);

		bool clientExists = false;
		for (const auto& client : myClients)
		{
			if (client.second == fromClient)
			{
				clientExists = true;
			}
		}

		if (!clientExists)
		{
			myClients.try_emplace(myNrOfClients++, fromClient);
		}

		//TODO: Make interpret cast of message to store messages instead of char[].
	}

	else if (static_cast<eMessageType>(myBuffer[0]) == eMessageType::Chat)
	{
		std::cout << "Accepted packet: " << static_cast<unsigned int>(myBuffer[0]) << ": " << Utils::DeSerializeMessageString(myBuffer) << std::endl;
	}
		ChatMessage msg = *reinterpret_cast<ChatMessage*>(&myBuffer);
		myReceivedMessages.push(msg);

	myHasReceived = true;
}

bool ONet::Server::ShutDown()
{
	if (myIsActive)
	{
		myIsActive = false;
		//Close socket.
		closesocket(myListenSocket);
		WSACleanup();
		return true;
	}

	return false;
}

bool ONet::Server::AcceptClient(const unsigned int aClientID)
{
	SOCKET clientSocket;

	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (clientSocket == INVALID_SOCKET)
	{
		closesocket(clientSocket);
		return false;
	}

	//myClientIDs.insert(std::pair<unsigned int, SOCKET>(aClientID, clientSocket));
	return true;
}

//int ONet::Server::ReceiveData(const unsigned int aClientID, char* aRecvBuffer)
//{
//	if (myClientIDs.find(aClientID) != myClientIDs.end())
//	{
//		SOCKET currentSocket = myClientIDs[aClientID];
//		const int result = ONet::NetworkService::ReceiveData(currentSocket, aRecvBuffer, MAX_PACKET_SIZE);
//
//		if (result == 0)
//		{
//			closesocket(currentSocket);
//			myClientIDs.erase(currentSocket);
//		}
//
//		return result;
//	}
//
//	return 0;
//}
