#include "RodSystem.h"
#include "TransformComponent.h"
#include "ParticleContactComponent.h"

namespace Reality
{
	RodSystem::RodSystem()
	{
		requireComponent<RodComponent>();
	}

	void RodSystem::Update(float deltaTime)
	{
		Color green = Color(0, 1, 0);
		Color red = Color(1, 0, 0);

		for (auto e : getEntities())
		{
			auto& rod = e.getComponent<RodComponent>();
			float currentLength = glm::length(rod.entityA.getComponent<TransformComponent>().position -
				rod.entityB.getComponent<TransformComponent>().position);

			getWorld().data.renderUtil->DrawSphere(rod.entityA.getComponent<TransformComponent>().position);
			getWorld().data.renderUtil->DrawSphere(rod.entityB.getComponent<TransformComponent>().position);

			Vector3 normal = glm::normalize(rod.entityB.getComponent<TransformComponent>().position -
				rod.entityA.getComponent<TransformComponent>().position);

			Color& clr = green;
			if (currentLength > rod.length)
			{
				auto contactEntity = getWorld().createEntity();
				contactEntity.addComponent<ParticleContactComponent>(
					rod.entityA,
					rod.entityB,
					0,
					normal,
					currentLength - rod.length );

			}
			else
			{
				auto contactEntity = getWorld().createEntity();
				contactEntity.addComponent<ParticleContactComponent>(
					rod.entityA,
					rod.entityB,
					0,
					-normal,
					rod.length + currentLength );
				clr = red;
			}

			getWorld().data.renderUtil->DrawLine(rod.entityA.getComponent<TransformComponent>().position,
				rod.entityB.getComponent<TransformComponent>().position, clr);

		}
	}
}
