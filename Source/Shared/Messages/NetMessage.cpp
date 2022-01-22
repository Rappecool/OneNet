#include "NetMessage.h"
#include <assert.h>


ONet::NetMessage::NetMessage(size_t aSize, const eMessageType& aMessageType)
	:myMsgType(aMessageType)
{

}
