#pragma once
#include "NetMessage.h"

#define MAX_NETMESSAGE_SIZE 512

namespace ONet
{
	class ChatMessage : public NetMessage
	{
	public:
		ChatMessage(const eMessageType& aMessageType = eMessageType::Chat);
		void SetMessage(const std::string& aMessage);
	protected:

	};
}