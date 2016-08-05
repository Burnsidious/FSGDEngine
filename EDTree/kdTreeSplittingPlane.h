#pragma once

#include "EDTreeNode.h"

class kdTreeSplittingPlane : public EDTreeInternalNodeData
{
	unsigned int axis;
	float offset;
public:
	unsigned int GetAxis(void) const { return axis; } 
	float GetOffset(void) const { return offset; }
	
	void SetAxis(unsigned int _axis){ axis = _axis; }
	void SetOffset(float _offset){ offset = _offset; }
};