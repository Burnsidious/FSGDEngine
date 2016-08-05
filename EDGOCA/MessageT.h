#pragma once

#include "IMessage.h"

namespace EDGOCA
{
	template<typename type>
	class MessageT : public IMessage
	{
	public:
		type value;

		MessageT(MessageNameKey nameKey, type val) : IMessage(nameKey), value(val) {}
		MessageT(MessageNameKey nameKey) : IMessage(nameKey) {}
	};
}