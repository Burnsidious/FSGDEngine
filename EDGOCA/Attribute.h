#pragma once

#include "IAttribute.h"

namespace EDGOCA
{
	template<typename type>
	class Attribute : public IAttribute
	{
		friend class ComponentSystem;

		Attribute<type>* clone(void) const
		{
			return new Attribute<type>(*this);
		}
		
		void copy(IAttribute* attrib)
		{
			value = ((Attribute<type>*)attrib)->value;
		}

	public:
		type value;

		Attribute(AttributeNameKey nameKey) : IAttribute(nameKey){}

		void ReadXML(TiXmlElement* valElement){}
	};
}