#pragma once



namespace EDGOCA
{
	typedef size_t MessageNameKey;

	class IBehavior;
	class IMessage;
	typedef void (*OnMessageProc)( IBehavior* invokingBehavior, IMessage* message );

	typedef std::pair<MessageNameKey, OnMessageProc> MessageHandler;
	typedef std::map<MessageNameKey, OnMessageProc> MessageMap;

	class IMessage
	{
		MessageNameKey nameKey;
	public:
		IMessage(MessageNameKey nameKey) : nameKey(nameKey) {}

		virtual ~IMessage(void){}

		inline MessageNameKey GetNameKey(void){ return nameKey; }
	};
}