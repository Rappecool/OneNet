#pragma once
#include <string>

#define MAX_BUFFER_SIZE 512
#define NET_MESSAGE_BUFFER_ID 10

namespace ONet
{
	//TODO: Handle RPC, make enum what kind of units a function should be called.
	//TODO: Inherit from short to make smaller.
	enum class eMessageType
	{
		None,
		Chat,
		CLIENT_Join, //Tell server we're establishing a new connection.
		CLIENT_Leave, //Tell server we're leaving.
		SERVER_JoinResult, //Tell client they've succesfully connected.

		COUNT
	};

	class NetMessage
	{
	public:
		NetMessage(const eMessageType& aMessageType = eMessageType::None);

		inline const char* GetBuffer() const { return myBuffer; }
	protected:
		eMessageType myMsgType = eMessageType::None;
		unsigned short myID = 1;
		unsigned short mySenderID = 2;
		unsigned short myTargetID = 3;
		short myBufferLength = -1;

		char myBuffer[MAX_BUFFER_SIZE];

	private:
	};


}