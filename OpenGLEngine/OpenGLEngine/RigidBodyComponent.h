#pragma once
#include "ECSConfig.h"

namespace Reality
{
	struct RigidBodyComponent
	{
		RigidBodyComponent()
		{

		}
		float inverseMass;
		Vector3 velocity;
		Vector3 rotation;
		Mat4 transformationMatrix;
	};
}
