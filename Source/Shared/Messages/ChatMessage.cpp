#include "ChatMessage.h"
#include <assert.h>

ONet::ChatMessage::ChatMessage(const eMessageType& aMessageType)
	:NetMessage(aMessageType)
{
}

void ONet::ChatMessage::SetMessage(const std::string& aMessage)
{
	assert(sizeof(char) * aMessage.size() <= MAX_BUFFER_SIZE && "Trying to send message that is bigger than max size!");
	myBufferLength = sizeof(char) * aMessage.size();
	memcpy(myBuffer, &aMessage[0], myBufferLength);
}