#pragma once
#include "ECSConfig.h"

#include "BuoyancyComponent.h"



namespace Reality
{
	class BuoyancyForceGeneratorSystem : public ECSSystem
	{
	public:
		BuoyancyForceGeneratorSystem();
		void Update(float deltaTime);
	};
}
