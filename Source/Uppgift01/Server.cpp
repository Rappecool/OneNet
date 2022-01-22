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
#include "..\Shared\Messages\ChatMessage.h"

ONet::Server& ONet::Server::GetInstance()
{
	static Server instance(10);
	return instance;
}

ONet::Server::Server(const unsigned short aMaxNrOfClients)
	:myMaxNrOfClients(aMaxNrOfClients)
{
	myListenSocket = INVALID_SOCKET;
	myClientSocket = INVALID_SOCKET;
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
	myListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in serverHint;
	//Get any open address on network card.
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_family = AF_INET;

	//htons converts how bytes are stored in memory. Converts from decimals to hexadecimal for network byte order.
	//htons = host-to-network short.
	serverHint.sin_port = htons(54000); //Convert from little to big endian.

	//TODO: Handle nonblocking sockets with no msg. (error 1035?)

	//Bind socket.
	//Socket is bound to port/IP in one.
	if (bind(myListenSocket, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		printf("Can't bind socket! %d", WSAGetLastError());
		return false;
	}

	return true;
}

void ONet::Server::Tick()
{
	//Create buffer for clients to receive.
	//Holds data for other clients connected to our server.
	sockaddr_in client;
	int clientLength = sizeof(client);
	ZeroMemory(&client, clientLength);

	const int bufferSize = sizeof(ChatMessage);
	char buffer[bufferSize];

	std::cout << "TICKING" << std::endl;
	
	ChatMessage msg;
	msg.SetMessage("SERVERMSG");

	//while (myIsActive)
	//{
		//Clear buffer every time we receive message.
		ZeroMemory(buffer, bufferSize);
		//Wait for message

		//receive from socket (in), to buffer (buffer), and put information in client (&client).
		//Fills in metadata in our buffer that client has sent.
		int bytesIn = recvfrom(myListenSocket, buffer, bufferSize, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from client: " << WSAGetLastError() << std::endl;
			//continue;
		}
		if (bytesIn <= 5)
		{
			//continue;
		}

		if (static_cast<eMessageType>(buffer[0]) == eMessageType::Chat)
		{
			bool found = true;
			std::cout << "Found chat message!" << std::endl;

			const short messageSize = buffer[10];

			std::string message = "";
			message.resize(messageSize);
			for (unsigned short charIndex = 0; charIndex < messageSize; charIndex++)
			{
				message[charIndex] = buffer[12 + charIndex];
			}

			std::cout << message << std::endl;
		}

		//Display message and client info.
		char clientIP[256];
		ZeroMemory(clientIP, 256);

		//AF_INET == IPV4., get information out of sin_addr, which is in a series of bytes, (4 bytes in our case) and convert to string (which is 10 bytes).
		//Interprets data, ntop = number to string, gets our client IP as a string.
		//inet_ntop(AF_INET, &client.sin_addr, clientIP, 256);

		//std::cout << "Message received from: " << clientIP << std::endl << ": " << buffer << std::endl;

		int sendSuccess = sendto(myListenSocket, buffer, bufferSize, 0, (sockaddr*)&client, clientLength);
		if (sendSuccess == SOCKET_ERROR)
		{
			std::cout << "Failed to send client data" << WSAGetLastError() << std::endl;
		}
		else
		{
			std::cout << "Successfully sent packet to: " << client.sin_port << std::endl;
		}

	//}

	//Close socket.
	//closesocket(myListenSocket);

	//WSACleanup();
}

void ONet::Server::Temp()
{
	std::cout << "TEMPTEMP" << std::endl;
}

bool ONet::Server::AcceptClient(const unsigned int aClientID)
{
	//Create connection to listen socket, huh?
	myClientSocket = accept(myListenSocket, nullptr, nullptr);

	if (myClientSocket != INVALID_SOCKET)
	{
		char value = 1;
		//Save clients to our map.
		myClientIDs.insert(std::pair<unsigned int, SOCKET>(aClientID + 1, myClientSocket));

		return true;
	}

	return false;
}

int ONet::Server::ReceiveData(const unsigned int aClientID, char* aRecvBuffer)
{
	if (myClientIDs.find(aClientID) != myClientIDs.end())
	{
		SOCKET currentSocket = myClientIDs[aClientID];
		const int result = ONet::NetworkService::ReceiveData(currentSocket, aRecvBuffer, MAX_PACKET_SIZE);

		if (result == 0)
		{
			closesocket(currentSocket);
			myClientIDs.erase(currentSocket);
		}

		return result;
	}

	return 0;
}
