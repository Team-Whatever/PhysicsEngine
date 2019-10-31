#include "BungeeForceGeneratorSystem.h"
#include "ParticleComponent.h"

namespace Reality
{
	BungeeForceGeneratorSystem::BungeeForceGeneratorSystem()
	{
		requireComponent<TransformComponent>();
		requireComponent<BungeeComponent>();
	}


	void BungeeForceGeneratorSystem::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			auto &spring = e.getComponent<BungeeComponent>();

			if (spring.entityA.hasComponent<TransformComponent>() &&
				spring.entityB.hasComponent<TransformComponent>())
			{
				auto &entityTransformA = spring.entityA.getComponent<TransformComponent>();
				auto &entityTransformB = spring.entityB.getComponent<TransformComponent>();

				Vector3 relativePosition = entityTransformA.position - entityTransformB.position;
				float length = glm::length(relativePosition);
				float x = length - spring.restLength;
				Vector3 direction = glm::normalize(relativePosition);

				// Calculate the vector of the spring.
				Vector3 force;
				force = spring.entityA.getComponent<TransformComponent>().position;
				force -= spring.entityB.getComponent<TransformComponent>().position;
				float magnitude = glm::length(force);

				if (magnitude <= spring.restLength) 
					return;

				magnitude = spring.springConstant * (spring.restLength - magnitude);
				force = glm::normalize(force);
				force *= -magnitude;

				if (spring.entityB.hasComponent<ParticleComponent>())
				{
					spring.entityB.getComponent<ParticleComponent>().AddForce(force);
				}

				float g = 1.0f / (1.0f + pow(abs(x), 0.5f));
				float r = (1 - g);
				Color color = Color(r, g, 0, 1);

				float deltaLength = length / 10.0f;
				for (int i = 0; i < 10; i++)
				{
					getWorld().data.renderUtil->DrawCube(
						entityTransformB.position + (float)i * deltaLength * direction,
						Vector3(0.1f, 0.1f, 0.1f) * min((spring.springConstant / 10.0f), 5.0f), Vector3(0, 0, 0), color);
				}

				getWorld().data.renderUtil->DrawLine(
					entityTransformB.position, entityTransformB.position + length * direction, color);
			}
		}

	}
}
