#include "precompiled.h"
#include "IBehavior.h"
#include "GameObject.h"

namespace EDGOCA
{
	IAttribute* IBehavior::GetAttribute(AttributeNameKey attributeNameKey)
	{
		return gameObject->GetAttribute(attributeNameKey);
	}
}