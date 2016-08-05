#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"
#include "../EDGameCore/GameObject.h"

#include "../EDUtilities/PythonModule.h"

class PythonBehavior : public EDGameCore::Behavior<PythonBehavior>
{
public:

	PythonBehavior();
	PythonBehavior(PythonBehavior& _rhs);
	PythonBehavior& operator=(PythonBehavior& _rhs);

	~PythonBehavior();

	void LateUpdate();
	void LoadState(TiXmlElement* xmlElement);

	PythonModule module;

private:

	string GetModuleName() { return module.GetModuleName(); }
	void Awake(void);

	std::string targetName;
	unsigned int targetId;
	EDGameCore::GameObject* target;
};
