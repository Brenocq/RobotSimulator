//--------------------------------------------------
// Atta Physics
// hingeConstraint.cpp
// Date: 2020-08-30
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "hingeConstraint.h"

namespace atta::phy
{
	HingeConstraint::HingeConstraint(glm::vec3 position, glm::vec3 rotation):
		_position(position), _rotation(rotation)
	{
		_type = "HingeConstraint";
	}

	HingeConstraint::~HingeConstraint()
	{
	}
}
