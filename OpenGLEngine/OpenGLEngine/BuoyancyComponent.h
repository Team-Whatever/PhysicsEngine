#pragma once
#include "ECSConfig.h"

namespace Reality
{
	struct BuoyancyComponent
	{
		BuoyancyComponent(float _maxDepth = 50, float _volume = 1000, float _waterHeight = 100, float _liquidDensity = 1000.0f, ECSEntity e = ECSEntity())
			:maxDepth(_maxDepth), volume(_volume), waterHeight(_waterHeight), liquidDensity(_liquidDensity), entity(e)
		{
		}

		// maximum submersion depth of the object
		// before it generates its maximum buoyancy force.
		float maxDepth;

		// the volume of the object
		float volume;

		// the height of the water plane above y = 0.
		// the plane will be parallel to the XZ plane.
		float waterHeight;

		// the density of the liquid.
		// pure water has a density of 100 kg per cubic menter.
		float liquidDensity;

		ECSEntity entity;

	};
}
