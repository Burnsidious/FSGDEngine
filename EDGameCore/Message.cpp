#include "Message.h"
#include "Game.h"

namespace EDGameCore
{
	Message::Message(const char* message)
	{
		messageId = Game::RegisterString(message);
	}
}