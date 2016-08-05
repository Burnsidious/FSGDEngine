
#include "precompiled.h"

#include "../EDGameCore/Transform.h"


namespace EDGameCore
{
	const Float4x4& Transform::_GetWorldMatrixSolution(void)
	{
		if( dirtied )
		{
			if( parent != 0 )
				Multiply( worldMatrix, localMatrix, parent->GetWorldMatrix() );
			else
				worldMatrix = localMatrix;

			dirtied = false;
		}

		return worldMatrix;
	}
}