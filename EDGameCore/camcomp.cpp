#include "camcomp.h"
#include "Camera.h"

namespace EDGameCore
{
	bool compstruct::operator()(const Camera* a, const Camera* b)const
	{
		return (a->GetDepth() < b->GetDepth() || ((a->GetDepth() == b->GetDepth()) && a < b));
	}
}