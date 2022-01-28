#include "NetMessage.h"
#include <assert.h>


ONet::NetMessage::NetMessage(const eMessageType& aMessageType)
	:myMsgType(aMessageType)
{
}
