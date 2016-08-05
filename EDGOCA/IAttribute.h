#pragma once

class TiXmlElement;

namespace EDGOCA
{
	typedef size_t AttributeTypeKey;
	typedef size_t AttributeNameKey;

	class IAttribute
	{
		friend class Scene;

		AttributeNameKey nameKey;

	protected:
		virtual IAttribute* clone(void) const = 0;
		virtual void copy(IAttribute*) = 0;

	public:
		IAttribute(AttributeNameKey nameKey) : nameKey(nameKey) {}
		
		virtual ~IAttribute(void) = 0 {}

		virtual void ReadXML(TiXmlElement* valElement) = 0;
	};
}