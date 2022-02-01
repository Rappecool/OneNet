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

	char buffer[DEFAULT_PACKET_SIZE];


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
			//ChatMessage msg;
			//msg.SetMessage(&myBuffer[0]);

			ChatMessage temp = *reinterpret_cast<ChatMessage*>(&myReceivedMessages.back());
			ZeroMemory(buffer, bufferSize);
			memcpy(buffer, &temp, bufferSize);

			int sendSuccess = sendto(myListenSocket, buffer, bufferSize, 0, (sockaddr*)&client, clientLength);
			//int sendSuccess = sendto(myListenSocket, temp.GetBuffer(), bufferSize, 0, (sockaddr*)&client, clientLength);
			if (sendSuccess == SOCKET_ERROR)
			{
				std::cout << "Failed to send client data to : " << myClientNames[clientIndex] << "." << WSAGetLastError() << std::endl;
			}
			else
			{
				std::cout << "Successfully sent packet to: " << myClients[clientIndex].GetPort() << myClientNames[clientIndex] << std::endl;
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
	ZeroMemory(myBuffer, DEFAULT_PACKET_SIZE);

	const int bufferSize = sizeof(ChatMessage);

	std::cout << "SERVER_ReceiveData()" << std::endl;

	//ZeroMemory(&myBuffer, bufferSize);
	const int bytesIn = recvfrom(myListenSocket, myBuffer, bufferSize, 0, (sockaddr*)&client, &clientLength);

	if (bytesIn == SOCKET_ERROR)
	{
		std::cout << "Error receiving from client: " << WSAGetLastError() << std::endl;
	}

	if (bytesIn <= 0)
	{
		return;
	}

	else if (static_cast<eMessageType>(myBuffer[0]) == eMessageType::CLIENT_Join)
	{
		ChatMessage msg = *reinterpret_cast<ChatMessage*>(&myBuffer);
		myReceivedMessages.push(msg);
		HandleClientJoin(client, msg);

	}
	else if (static_cast<eMessageType>(myBuffer[0]) == eMessageType::CLIENT_Leave)
	{
		HandleClientLeave(client);
	}

	else if (static_cast<eMessageType>(myBuffer[0]) == eMessageType::Chat)
	{
		std::cout << "Accepted packet: " << static_cast<unsigned int>(myBuffer[0]) << ": " << Utils::DeSerializeMessageString(myBuffer) << std::endl;

		ChatMessage msg = *reinterpret_cast<ChatMessage*>(&myBuffer);
		myReceivedMessages.push(msg);
	}

	myHasReceived = true;
}

void ONet::Server::HandleClientJoin(sockaddr_in& client, const ChatMessage& aMsg)
{
	assert(myNrOfClients < myMaxNrOfClients && "Exceeded max nr of clients!");
	std::cout << "Found new client! ID: " << myNrOfClients << std::endl;
	IPEndPoint fromClient(std::to_string(client.sin_addr.S_un.S_addr), client.sin_port);

	char buffer[DEFAULT_PACKET_SIZE];
	ZeroMemory(buffer, DEFAULT_PACKET_SIZE);
	memcpy(buffer, &aMsg, DEFAULT_PACKET_SIZE);

	const auto clientName = Utils::DeSerializeMessageString(buffer);

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
		myClientNames.try_emplace(myNrOfClients, clientName);
	}
}

void ONet::Server::HandleClientLeave(sockaddr_in& client)
{
	IPEndPoint fromClient(std::to_string(client.sin_addr.S_un.S_addr), client.sin_port);

	bool clientExists = false;
	for (int clientIndex = myClients.size() - 1; clientIndex >= 0; clientIndex--)
	{
		if (myClients[clientIndex] == fromClient)
		{
			clientExists = true;
			//TODO: TEst
			ChatMessage clientLeaveMsg;
			clientLeaveMsg.SetMessage("Client " + myClientNames[clientIndex] + " left. Index: " + std::to_string((myNrOfClients)));
			myReceivedMessages.push(clientLeaveMsg);

			myClients.erase(clientIndex);
			myClientNames.erase(clientIndex);
			std::cout << "Successfully removed client: " << myClientNames[clientIndex] << std::endl;
			myNrOfClients--;
			
			continue;
		}
	}
	if (!clientExists)
	{
		std::cout << "Tried removing client that doesn't exist." << std::endl;
	}
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