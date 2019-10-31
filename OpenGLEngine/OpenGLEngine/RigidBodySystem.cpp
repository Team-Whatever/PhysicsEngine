#include "RigidBodySystem.h"

namespace Reality
{
	RigidBodySystem::RigidBodySystem()
	{
		requireComponent<RigidBodyComponent>();
	}

	void RigidBodySystem::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			
		}
	}
}
