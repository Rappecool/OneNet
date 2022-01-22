#include "Client.h"
#include "..\Shared\Networking\NetworkUtils.h"
#include "..\Shared\Messages\ChatMessage.h"
#include "..\Shared\Messages\MessageUtils.h"

ONet::Client::Client(const unsigned short aID, const std::string& aName)
	:myID(aID), myName(aName)
{
	myID++;
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

	//Create a hint structure for myServerAddress.
	myServerAddress.sin_family = AF_INET;
	myServerAddress.sin_port = htons(54000);
	int myServerAddressLength = sizeof(myServerAddress);

	inet_pton(AF_INET, "127.0.0.1", &myServerAddress.sin_addr);

	//Socket creation.
	mySocket = socket(AF_INET, SOCK_DGRAM, 0);

	int success = connect(mySocket, (sockaddr*)&myServerAddress, sizeof(myServerAddress));

	if (success == SOCKET_ERROR)
	{
		std::cout << "Failed to connect to serverAddress: " << WSAGetLastError() << std::endl;
		return false;
	}
	else
	{
		std::cout << "Succesfully connected to ServerAddress: " << myServerAddress.sin_port << std::endl;
	}

	return true;
}

void ONet::Client::Tick()
{
	int serverAddressLength = sizeof(myServerAddress);

	const int bufferSize = sizeof(ChatMessage);
	char msgOutBuffer[bufferSize];
	ZeroMemory(msgOutBuffer, bufferSize);

	while (myName.size() < 5)
	{
		std::cin >> myName;
		std::cin.clear();

		ChatMessage msg;
		msg.SetMessage(myName);
		memcpy(msgOutBuffer, &msg, bufferSize);

		int sendSuccess = sendto(mySocket, msgOutBuffer, bufferSize, 0, (sockaddr*)&myServerAddress, serverAddressLength);
		if (sendSuccess == SOCKET_ERROR)
		{
			std::cout << "Failed to send client data" << WSAGetLastError() << std::endl;
		}
		else
		{
			std::cout << "Successfully sent packet to: " << myServerAddress.sin_port << std::endl;
		}

		ZeroMemory(msgOutBuffer, bufferSize);

		int bytesIn = recvfrom(mySocket, msgOutBuffer, bufferSize, 0, (sockaddr*)&myServerAddress, &serverAddressLength);

		//Display message and client info.
		char serverAddressIP[256];
		ZeroMemory(serverAddressIP, 256);

		//AF_INET == IPV4., get information out of sin_addr, which is in a series of bytes, (4 bytes in our case) and convert to string (which is 10 bytes).
		//Interprets data, ntop = number to string, gets our client IP as a string.
		inet_ntop(AF_INET, &myServerAddress.sin_addr, serverAddressIP, 256);

		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from myServerAddress: " << WSAGetLastError() << std::endl;
		}
		else
		{
			std::cout << "Message received from: " << serverAddressIP << std::endl << ": " << Utils::DeSerializeMessageString(msgOutBuffer) << std::endl;
		}
	}

	//close socket.
	closesocket(mySocket);

}
