#pragma once
#include "ECSConfig.h"
#include "TransformComponent.h"
#include "BungeeComponent.h"

namespace Reality
{
	class BungeeForceGeneratorSystem : public ECSSystem
	{
	public:
		BungeeForceGeneratorSystem();
		void Update(float deltaTime);
	};
}

