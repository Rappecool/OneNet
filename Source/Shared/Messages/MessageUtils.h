#pragma once
#include <string>
#include "ChatMessage.h"

namespace ONet
{
	namespace Utils
	{
		static const std::string DeSerializeMessageString(const char aBuffer[], const short aBufferSize = sizeof(ChatMessage))
		{
			std::string message = "";
			if (static_cast<eMessageType>(aBuffer[0]) == eMessageType::Chat)
			{
				bool found = true;

				//10, since that is the location in bytes that we store message length.
				const short messageSize = aBuffer[NET_MESSAGE_BUFFER_ID];

				message.resize(messageSize);
				for (unsigned short charIndex = 0; charIndex < messageSize; charIndex++)
				{
					message[charIndex] = aBuffer[12 + charIndex];
				}
			}

			else
			{
				message = "ERROR";
			}

			return message;
		}
	}
}