#include "Client.h"
#include "..\Shared\Networking\NetworkUtils.h"
#include "..\Shared\Messages\ChatMessage.h"
#include "..\Shared\Messages\MessageUtils.h"

ONet::Client::Client(const unsigned int aPort, const std::string& aAdress, const std::string& aName)
	:myName(aName)
{
	myIPEndpoint = IPEndPoint(aAdress, aPort);
}

bool ONet::Client::Init()
{
	//startup Winsock.
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsSuccess = WSAStartup(version, &data);
	if (wsSuccess != 0)
	{
		assert(false && "Failed to init myServerAddress_Winsock");
	}

	bool result = ConnectToServer();
	myIsActive = true;

	ZeroMemory(myReceiveBuffer, DEFAULT_PACKET_SIZE);
	ZeroMemory(mySendBuffer, DEFAULT_PACKET_SIZE);

	return result;
}

void ONet::Client::Tick()
{
	int serverAddressLength = sizeof(myServerAddress);
	const int bufferSize = sizeof(ChatMessage);

	while (myIsActive && myHasEstablishedConnection && !myWantsToQuit)
	{
		std::cout << "CLIENT_TICK" << std::endl;

		ZeroMemory(mySendBuffer, DEFAULT_PACKET_SIZE);
		std::string inString = "";
		std::cin >> inString;
		std::cin.clear();

		//ZeroMemory(buffer, DEFAULT_PACKET_SIZE);

		ChatMessage msg(eMessageType::Chat);
		if (inString == "quit")
		{
			msg = ChatMessage(eMessageType::CLIENT_Leave);
			myWantsToQuit = true;
		}

		msg.SetMessage(myName + " says : " + inString);
		memcpy(mySendBuffer, &msg, (int)DEFAULT_PACKET_SIZE);

		const int sendSuccess = sendto(mySocket, mySendBuffer, bufferSize, 0, (sockaddr*)&myServerAddress, serverAddressLength);
		if (sendSuccess == SOCKET_ERROR)
		{
			std::cout << "Failed to send client data" << WSAGetLastError() << std::endl;
		}
		else
		{
			ZeroMemory(mySendBuffer, DEFAULT_PACKET_SIZE);
			std::cout << "Successfully sent packet to: " << myServerAddress.sin_port << std::endl;
		}

		//Check each loop if we want to quit, then set not active to not call thread tick.
		if (myWantsToQuit)
		{
			myIsActive = false;
		}
	}
}

void ONet::Client::ReceiveData()
{
	ZeroMemory(myReceiveBuffer, DEFAULT_PACKET_SIZE);

	int serverAddressLength = sizeof(myServerAddress);
	const int bytesIn = recvfrom(mySocket, myReceiveBuffer, DEFAULT_PACKET_SIZE, 0, (sockaddr*)&myServerAddress, &serverAddressLength);

	if (bytesIn > 0)
	{
		myHasReceived = true;
		char serverAddressIP[256];
		ZeroMemory(serverAddressIP, 256);
		inet_ntop(AF_INET, &myServerAddress.sin_addr, serverAddressIP, 256);

		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from myServerAddress: " << WSAGetLastError() << std::endl;
		}

		if (myReceiveBuffer[0] == static_cast<int>(eMessageType::CLIENT_Join))
		{
			myHasEstablishedConnection = true;
			std::cout << "Client has established connection" << std::endl;
		}

		else if (myReceiveBuffer[0] == static_cast<int>(eMessageType::Chat))
		{
			std::cout << "Message received from: " << serverAddressIP << std::endl << ": " << Utils::DeSerializeMessageString(myReceiveBuffer) << std::endl;
		}

		else
		{
			std::cout << "Client received unhandled message" << std::endl;
		}
	}
}

bool ONet::Client::ShutDown()
{
	closesocket(mySocket);
	return true;
}

bool ONet::Client::ConnectToServer()
{
	//Create a hint structure for myServerAddress.

	myServerAddress.sin_family = AF_INET;
	myServerAddress.sin_port = htons(myIPEndpoint.GetPort());
	//myServerAddress.sin_port = htons(port);
	const int serverAddressLength = sizeof(myServerAddress);

	//std::string address = "127.0.0.1";
	inet_pton(AF_INET, myIPEndpoint.GetAdress().c_str(), &myServerAddress.sin_addr);

	//Socket creation.
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	ChatMessage msg(eMessageType::CLIENT_Join);
	msg.SetMessage(myName);

	char buffer[DEFAULT_PACKET_SIZE];
	memcpy(&buffer, &msg, DEFAULT_PACKET_SIZE);

	const int sendSuccess = sendto(mySocket, buffer, DEFAULT_PACKET_SIZE, 0, (sockaddr*)&myServerAddress, serverAddressLength);

	return true;
}
