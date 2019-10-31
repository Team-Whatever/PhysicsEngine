#include "BuoyancyForceGeneratorSystem.h"
#include "ParticleComponent.h"
#include "TransformComponent.h"

namespace Reality
{
	BuoyancyForceGeneratorSystem::BuoyancyForceGeneratorSystem()
	{
		requireComponent<BuoyancyComponent>();
		requireComponent<TransformComponent>();
	}

	void BuoyancyForceGeneratorSystem::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			auto& buoyancy = e.getComponent<BuoyancyComponent>();
			auto& volumeTransform = e.getComponent<TransformComponent>();
			if (buoyancy.entity.hasComponent<ParticleComponent>() && 
				buoyancy.entity.hasComponent<TransformComponent>() )
			{
				auto& particle = buoyancy.entity.getComponent<ParticleComponent>();
				auto& transform = buoyancy.entity.getComponent<TransformComponent>();

				// calculate the submersion depth
				float depth = transform.position.y;

				// check if we're out of the water.
				if (depth >= buoyancy.waterHeight + buoyancy.maxDepth)
					return;

				Vector3 force(0, 0, 0);

				// check if we're at maximum depth.
				if (depth <= buoyancy.waterHeight - buoyancy.maxDepth)
				{
					force.y = buoyancy.liquidDensity * buoyancy.volume;
					particle.AddForce(force);
				}
				else
				{
					// otherwise we are partly submerged.
					force.y = buoyancy.liquidDensity * buoyancy.volume *
						(depth - buoyancy.maxDepth - buoyancy.waterHeight) / ( 2 * buoyancy.maxDepth );
					particle.AddForce(force);
				}

				getWorld().data.renderUtil->DrawSphere(transform.position);
			}

			getWorld().data.renderUtil->DrawCube(volumeTransform.position, volumeTransform.scale, volumeTransform.eulerAngles );
		}
	}
}
