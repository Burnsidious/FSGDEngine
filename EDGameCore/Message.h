#pragma once
#include "Registry.h"

namespace EDGameCore
{
	class Message
	{
	public:
		Message(RegistrationId id) : messageId(id) {}
		Message(const char* message);

		virtual ~Message(){}
		
		RegistrationId messageId;
	};

	template<typename T>
	class MessageT : public Message
	{
	public:
		MessageT(RegistrationId id) : Message(id) {}
		MessageT(const char* message) : Message(message) {}
		MessageT(RegistrationId id, T val) : Message(id), value(val) {}
		MessageT(const char* message, T val) : Message(message), value(val) {}

		T value;	
	};
}