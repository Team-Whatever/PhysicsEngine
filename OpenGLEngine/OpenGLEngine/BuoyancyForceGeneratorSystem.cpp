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
		GLFWwindow*  window = getWorld().data.renderUtil->window->glfwWindow;

		for (auto e : getEntities())
		{
			auto& buoyancy = e.getComponent<BuoyancyComponent>();
			auto& volumeTransform = e.getComponent<TransformComponent>();

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
				buoyancy.liquidDensity += 10;
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
				buoyancy.liquidDensity -= 10;

			if (buoyancy.entity.hasComponent<ParticleComponent>() && 
				buoyancy.entity.hasComponent<TransformComponent>() )
			{
				auto& particle = buoyancy.entity.getComponent<ParticleComponent>();
				auto& transform = buoyancy.entity.getComponent<TransformComponent>();

				// calculate the submersion depth
				float depth = transform.position.y;

				Vector3 force(0, 0, 0);
				// check if we're out of the water.
				if (depth >= buoyancy.waterHeight + buoyancy.maxDepth)
				{
					// no force add
				}
				// check if we're at maximum depth.
				else if (depth <= buoyancy.waterHeight - buoyancy.maxDepth)
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
			getWorld().data.renderUtil->RenderText("Density : " + std::to_string((int)buoyancy.liquidDensity), 10.0f, 1060.0f, 0.5f, Color(0, 1, 1, 1));
			getWorld().data.renderUtil->RenderText("Volume : " + std::to_string((int)buoyancy.volume), 10.0f, 1030.0f, 0.5f, Color(0, 1, 1, 1));
			getWorld().data.renderUtil->RenderText("MaxDepth : " + std::to_string((int)buoyancy.maxDepth), 10.0f, 1000.0f, 0.5f, Color(0, 1, 1, 1));
			getWorld().data.renderUtil->RenderText("WaterHeight : " + std::to_string((int)buoyancy.waterHeight), 10.0f, 970.0f, 0.5f, Color(0, 1, 1, 1));
		}

		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
			AddNewBuoyancyComponent();
	}

	void BuoyancyForceGeneratorSystem::AddNewBuoyancyComponent()
	{
		Camera& camera = getWorld().data.renderUtil->camera;
		Vector3 newPos = camera.Position + camera.Front * 10.0f;
		Vector3 scale = Vector3(30, 30, 30);

		auto entity = getWorld().createEntity();
		entity.addComponent<TransformComponent>(newPos);
		entity.addComponent<ParticleComponent>();

		auto b = getWorld().createEntity();
		b.addComponent<TransformComponent>(newPos, scale);
		b.addComponent<BuoyancyComponent>(scale.y * 0.5f, 10, scale.y, 100, entity);
	}
}
