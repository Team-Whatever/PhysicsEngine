#pragma once
#include "ECSConfig.h"
#include "RigidBodyComponent.h"

namespace Reality
{
	class RigidBodySystem : public ECSSystem
	{
	public:
		RigidBodySystem();
		void Update(float deltaTime);
	};
}
