//#define STB_IMAGE_IMPLEMENTATION
#include "RenderingSystem.h"
#include "InputEventSystem.h"
#include "RotateSystem.h"
#include "ParticleSystem.h"
#include "ParticleSpawnerSystem.h"
#include "GravityForceGeneratorSystem.h"
#include "FixedSpringForceGeneratorSystem.h"
#include "ForceAccumulatorSystem.h"
#include "PairedSpringForceGeneratorSystem.h"
#include "SphereContactGeneratorSystem.h"
#include "ParticleContactResolutionSystem.h"
#include "CableComponentSystem.h"
#include "RodSystem.h"
#include "FPSControlSystem.h"
#include "DynamicDirectionalLightSystem.h"
#include "DynamicPointLightSystem.h"
#include "DynamicSpotLightSystem.h"
#include <string>
#include <stdlib.h>     
#include <time.h>       

#define DEBUG_LOG_LEVEL 3
#define RANDOM_FLOAT(LO, HI) LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))

using namespace Reality;

void LoadShaders(ECSWorld& world);
void LoadModels(ECSWorld& world);
void MakeABunchaObjects(ECSWorld& world);
void MakeABunchaSprings(ECSWorld& world);
void MakeABunchaSpheres(ECSWorld& world);
void MakeACable(ECSWorld& world);
void MakeCablesAndRods(ECSWorld& world);
void SetupLights(ECSWorld& world);

int main()
{
	ECSWorld world;

	// Init and Load
	world.data.InitRendering();
	//LoadAssets(world);
	
	world.data.renderUtil->camera.Position = Vector3(0, 15.0f, 100.0f);

	// Create entities

	// Make a player controller
	auto e = world.createEntity();
	e.addComponent<FPSControlComponent>();

	auto wall = world.createEntity();
	wall.addComponent<TransformComponent>(Vector3(0, -3.0f, 0.0f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0, 270, 0));
	// Add mesh
	wall.addComponent<ModelComponent>("Resources/Models/Sponza-master/sponza.obj");

	SetupLights(world);
	MakeABunchaObjects(world);
	//MakeABunchaSpheres(world);
	//MakeABunchaSprings(world);
	//MakeACable(world);
	MakeCablesAndRods(world);

	// Create Systems
	world.getSystemManager().addSystem<RenderingSystem>();
	world.getSystemManager().addSystem<InputEventSystem>();
	world.getSystemManager().addSystem<RotateSystem>();
	world.getSystemManager().addSystem<ParticleSystem>();
	world.getSystemManager().addSystem<ParticleSpawnerSystem>();
	world.getSystemManager().addSystem<GravityForceGeneratorSystem>();
	world.getSystemManager().addSystem<FixedSpringForceGeneratorSystem>();
	world.getSystemManager().addSystem<PairedSpringForceGeneratorSystem>();
	world.getSystemManager().addSystem<SphereContactGeneratorSystem>();
	world.getSystemManager().addSystem<CableComponentSystem>();
	world.getSystemManager().addSystem<ParticleContactResolutionSystem>();
	world.getSystemManager().addSystem<ForceAccumulatorSystem>();
	world.getSystemManager().addSystem<FPSControlSystem>();
	world.getSystemManager().addSystem<RodSystem>();
	world.getSystemManager().addSystem<DynamicDirectionalLightSystem>();
	world.getSystemManager().addSystem<DynamicPointLightSystem>();
	world.getSystemManager().addSystem<DynamicSpotLightSystem>();

	float time = glfwGetTime();
	float stepTime = glfwGetTime();
	float deltaTime = 0;
	float elapsedDeltaTime = 0;
	float logicDelta = 0;
	float debugDelta = 0;
	bool spawn = false;
	auto player = world.createEntity();

	LoadShaders(world);
	bool shadersLoaded = false;
	bool modelsLoadStarted = false;
	// game loop
	// -----------
	while (!glfwWindowShouldClose(world.data.renderUtil->window->glfwWindow))
	{
		float current = glfwGetTime();
		deltaTime = current - time;
		time = glfwGetTime();

		world.update();

		// Poll OpenGl events
		glfwPollEvents();

		world.data.renderUtil->ClearDisplay(world.data.renderUtil->window->glfwWindow);

		// Load
		if (!shadersLoaded)
		{
			shadersLoaded = world.data.assetLoader->ShadersLoaded();
		}
		if(shadersLoaded && !modelsLoadStarted)
		{
			LoadModels(world);
			modelsLoadStarted = true;
		}
		// Process Input
		world.getSystemManager().getSystem<InputEventSystem>().Update(deltaTime);

		// Game Logic Update
		world.getSystemManager().getSystem<FPSControlSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RotateSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<ParticleSpawnerSystem>().Update(deltaTime);

		// Physics
		//float fixedDeltaTime = glfwGetKey(world.data.renderUtil->window->glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS ? 1 / 60.0f : 0;		
		float fixedDeltaTime = 1 / 60.0f;
		// Force Generators
		world.getSystemManager().getSystem<GravityForceGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<FixedSpringForceGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<PairedSpringForceGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<SphereContactGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<CableComponentSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<RodSystem>().Update(fixedDeltaTime); 

		// Physics Solvers
		world.getSystemManager().getSystem<ForceAccumulatorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<ParticleContactResolutionSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<ParticleSystem>().Update(fixedDeltaTime);

		// Rendering Update
		world.getSystemManager().getSystem<DynamicDirectionalLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<DynamicPointLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<DynamicSpotLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RenderingSystem>().Update(deltaTime);

		elapsedDeltaTime = glfwGetTime() - time;
		logicDelta = elapsedDeltaTime - world.data.renderUtil->GetRenderDelta();
		stepTime = glfwGetTime();

		// Debug
		if (DEBUG_LOG_LEVEL > 0)
		{
			world.data.renderUtil->RenderText("FPS : " + std::to_string((int)round(1.0f / deltaTime)), 1810.0f, 1060.0f, 0.5f, Color(0, 1, 1, 1));
		}
		if (DEBUG_LOG_LEVEL > 1)
		{
			int logic = (int)round(logicDelta * 100.0f / deltaTime);
			std::string logicString = logic < 10 ? " " + std::to_string(logic) : std::to_string(logic);
			int render = (int)round(world.data.renderUtil->GetRenderDelta() * 100.0f / deltaTime);
			std::string renderString = logic < 10 ? " " + std::to_string(render) : std::to_string(render);
			int debug = (int)round(debugDelta * 100.0f / deltaTime);
			std::string debugString = logic < 10 ? " " + std::to_string(debug) : std::to_string(debug);
			
			world.data.renderUtil->RenderText("Logic : " + logicString + "%" +
				//+ " | Physics : " + std::to_string((int)round(physicsDelta * 100.0f / deltaTime)) + "%" +
				+ " | Rendering : " + renderString + "%" +
				+ " | Debug : " + debugString + "%"
				, 1680.0f, 1040.0f, 0.25f, Color(0, 1, 1, 1));
		}
		if (DEBUG_LOG_LEVEL > 2)
		{
			world.data.renderUtil->RenderText("Draw Calls : " + std::to_string(world.data.renderUtil->GetDrawCalls())
				+ " | Verts : " + std::to_string(world.data.renderUtil->GetVerts())
				+ " | Tris : " + std::to_string(world.data.renderUtil->GetTris())
				+ " | Lines : " + std::to_string(world.data.renderUtil->GetLines())
				, 1610.0f, 1020.0f, 0.25f, Color(0, 1, 1, 1));
		}

		// Update debug delta
		debugDelta = glfwGetTime() - stepTime;
		stepTime = glfwGetTime();

		world.data.renderUtil->SwapBuffers(world.data.renderUtil->window->glfwWindow);

		// Show FPS in console
		//std::cout << "FPS : " << 1.0f / deltaTime << std::endl;




		if (glfwGetKey(world.data.renderUtil->window->glfwWindow, GLFW_KEY_E) == GLFW_PRESS && !spawn)
		{
			spawn = true; 
			player.kill();
			player = world.createEntity(); 
			player.addComponent<TransformComponent>(Vector3(RANDOM_FLOAT(-20.0F, 20.0F), 40, 5)); 
			player.addComponent<SphereComponent>(1); 
			player.addComponent<ParticleComponent>(5, Vector3(0,0,0), 1);


			std::cout << "ball supposed to drop..." << std::endl; 

		}
		else if (glfwGetKey(world.data.renderUtil->window->glfwWindow, GLFW_KEY_E) == GLFW_RELEASE) {
			spawn = false;
		}

	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void LoadShaders(ECSWorld& world)
{
	world.data.assetLoader->StartShaderLoading({ {"Shaders/Lighting_Maps.vs", "Shaders/Lighting_Maps.fs"} });
}
void LoadModels(ECSWorld& world)
{
	world.data.assetLoader->StartModelLoading({
		"Resources/Models/Sponza-master/sponza.obj",
		"Resources/Models/nanosuit/nanosuit.obj"
		//"Resources/Models/ribbon-ball/model.obj",
		//"Resources/Models/animeclassroom/anime school.obj",
		//"Resources/Models/bagan-khayiminga-temple-interior/model.obj",
		//"Resources/Models/Robot/source/Zbot_Animation.fbx"
		});
}

void MakeABunchaObjects(ECSWorld& world)
{
	auto e = world.createEntity();
	e.addComponent<TransformComponent>(Vector3(4, -3.3f, 48), Vector3(1.0f, 1.0f, 1.0f), Vector3(0, 180, 0));
	// Add mesh
	e.addComponent<ModelComponent>("Resources/Models/nanosuit/nanosuit.obj");
	e.addComponent<RotateComponent>(0, 40, 0);

	e = world.createEntity();
	e.addComponent<TransformComponent>(Vector3(4, -3.3f, -62), Vector3(1.0f, 1.0f, 1.0f), Vector3(0, 0, 0));
	// Add mesh
	e.addComponent<ModelComponent>("Resources/Models/nanosuit/nanosuit.obj");
	e.addComponent<RotateComponent>(0, 40, 0);
}

void MakeABunchaSprings(ECSWorld& world)
{
	auto e = world.createEntity();
	float yOffset = 30;
	e.addComponent<TransformComponent>(Vector3(-2.5f, -5 + yOffset, -3), Vector3(1.0f, 1.0f, 1.0f));
	e.addComponent<ParticleComponent>();
	// Add mesh
	e.addComponent<ModelComponent>("Resources/Models/nanosuit/nanosuit.obj");

	auto springEntinty = world.createEntity();
	springEntinty.addComponent<TransformComponent>(Vector3(-2.5f, 0 + yOffset, 3));
	springEntinty.addComponent<FixedSpringComponent>(8, 2, e);

	auto e2 = world.createEntity();
	e2.addComponent<TransformComponent>(Vector3(2.5f, -5 + yOffset, -1), Vector3(1.0f, 1.0f, 1.0f));
	e2.addComponent<ParticleComponent>();
	// Add mesh
	e2.addComponent<ModelComponent>("Resources/Models/nanosuit/nanosuit.obj");

	auto springEntinty2 = world.createEntity();
	springEntinty2.addComponent<TransformComponent>(Vector3(2.5f, 0 + yOffset, 1));
	springEntinty2.addComponent<FixedSpringComponent>(5, 5, e2);

	auto pairedSpring = world.createEntity();
	pairedSpring.addComponent<PairedSpringComponent>(100, 5.0f, e, e2);

	auto e3 = world.createEntity();
	e3.addComponent<TransformComponent>(Vector3(-7.5f, -7.5f + yOffset, 1), Vector3(1.0f, 1.0f, 1.0f));
	e3.addComponent<ParticleComponent>();
	// Add mesh
	e3.addComponent<ModelComponent>("Resources/Models/nanosuit/nanosuit.obj");

	auto springEntinty3 = world.createEntity();
	springEntinty3.addComponent<TransformComponent>(Vector3(-7.5f, -10 + yOffset, -1));
	springEntinty3.addComponent<FixedSpringComponent>(7, 7, e3);

	auto e4 = world.createEntity();
	e4.addComponent<TransformComponent>(Vector3(7.5f, -7.5f + yOffset, 3), Vector3(1.0f, 1.0f, 1.0f));
	e4.addComponent<ParticleComponent>();
	// Add mesh
	e4.addComponent<ModelComponent>("Resources/Models/nanosuit/nanosuit.obj");

	auto springEntinty4 = world.createEntity();
	springEntinty4.addComponent<TransformComponent>(Vector3(7.5f, -10 + yOffset, -3));
	springEntinty4.addComponent<FixedSpringComponent>(5, 0, e4);

	auto pairedSpring2 = world.createEntity();
	pairedSpring2.addComponent<PairedSpringComponent>(100, 5.2f, e, e3);

	auto pairedSpring3 = world.createEntity();
	pairedSpring3.addComponent<PairedSpringComponent>(100, 5.2f, e2, e4);

	auto pairedSpring4 = world.createEntity();
	pairedSpring4.addComponent<PairedSpringComponent>(100, 10.0f, e3, e4);
}

void MakeABunchaSpheres(ECSWorld& world)
{
	for (int i = 0; i < 30; i++)
	{
		auto e = world.createEntity();
		//e.addComponent<TransformComponent>(Vector3(RANDOM_FLOAT(-1, 1), 20,0));

		e.addComponent<TransformComponent>(Vector3(RANDOM_FLOAT(-15.0f, 15.0f), RANDOM_FLOAT(6.0f, 34.0f), RANDOM_FLOAT(-15.0f, 15.0f)));
		e.addComponent<ParticleComponent>(1, Vector3(RANDOM_FLOAT(-5, 5), RANDOM_FLOAT(-5, 5), RANDOM_FLOAT(-5, 5))); 
		e.addComponent<SphereComponent>(1);
		Color col = Color(0, RANDOM_FLOAT(0.0f, 1.0f), RANDOM_FLOAT(0.0f, 1.0f));
		//e.addComponent<DynamicPointLightComponent>(20.0f, col, col, col);
	}

	auto ref = world.createEntity();
	ref.addComponent<TransformComponent>(Vector3(0, 20, 0), Vector3(0.3f, 0.3f, 0.3f), Vector3(0, 180, 0));
	// Add mesh
	ref.addComponent<ModelComponent>("Resources/Models/nanosuit/nanosuit.obj");
	ref.addComponent<RotateComponent>(0, 40, 0);
}

void MakeACable(ECSWorld& world)
{
	auto e1 = world.createEntity();
	e1.addComponent<TransformComponent>(Vector3(0, 40, 0));
	//e1.addComponent<ParticleComponent>(1, Vector3(0,0,0), 0);
	

	auto e2 = world.createEntity();
	e2.addComponent<TransformComponent>(Vector3(0, 30, 0));
	e2.addComponent<ParticleComponent>(1);
	
	auto e = world.createEntity();
	e.addComponent<CableComponent>(e1, e2, 20);
}

void MakeCablesAndRods(ECSWorld & world)
{
	//world.eFixed[27];
	
	/*
	for (int i = 0; i < 8; i++) {
		auto eFixed = world.createEntity();
		eFixed.addComponent<TransformComponent>(Vector3(30, 40, 0));
		eFixed.addComponent<SphereComponent>(1);
		eFixed.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);

	}*/


	//location:  farthest point to the right, away from camera starting position
	/*
	auto eFixed = world.createEntity();
	eFixed.addComponent<TransformComponent>(Vector3(30, 40, 0)); 
	eFixed.addComponent<SphereComponent>(1);
	eFixed.addComponent<ParticleComponent>(10000000, Vector3(0,0,0), 0); 
	//eFixed.addComponent<ParticleComponent>(1, Vector3(0,0,0), 0);

	auto eLowerPoint1 = world.createEntity();
	eLowerPoint1.addComponent<TransformComponent>(Vector3(30, 30, 0));
	eLowerPoint1.addComponent<SphereComponent>(1);
	eLowerPoint1.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable1 = world.createEntity();
	eCable1.addComponent<CableComponent>(eFixed, eLowerPoint1, 10);
	*/


	// ------------------------------------------------------------------------
	auto eFixed2 = world.createEntity();
	eFixed2.addComponent<TransformComponent>(Vector3(20, 40, 0)); 
	eFixed2.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed2.addComponent<SphereComponent>(1);

	auto eLowerPoint2 = world.createEntity();
	eLowerPoint2.addComponent<TransformComponent>(Vector3(20, 30, 0)); 
	eLowerPoint2.addComponent<SphereComponent>(1); 
	eLowerPoint2.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable2 = world.createEntity();
	eCable2.addComponent<CableComponent>(eFixed2, eLowerPoint2, 10);


	// ------------------------------------------------------------------------
	auto eFixed3 = world.createEntity();
	eFixed3.addComponent<TransformComponent>(Vector3(10, 40, 0));
	eFixed3.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed3.addComponent<SphereComponent>(1);

	auto eLowerPoint3 = world.createEntity();
	eLowerPoint3.addComponent<TransformComponent>(Vector3(10, 30, 0)); 
	eLowerPoint3.addComponent<SphereComponent>(1);
	eLowerPoint3.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable3 = world.createEntity();
	eCable3.addComponent<CableComponent>(eFixed3, eLowerPoint3, 10);


	// ------------------------------------------------------------------------
	auto eFixed4 = world.createEntity();
	eFixed4.addComponent<TransformComponent>(Vector3(0, 40, 0));
	eFixed4.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed4.addComponent<SphereComponent>(1);

	auto eLowerPoint4 = world.createEntity();
	eLowerPoint4.addComponent<TransformComponent>(Vector3(0, 30, 0));
	eLowerPoint4.addComponent<SphereComponent>(1);
	eLowerPoint4.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable4 = world.createEntity();
	eCable4.addComponent<CableComponent>(eFixed4, eLowerPoint4, 10);


	// ------------------------------------------------------------------------
	auto eFixed5 = world.createEntity();
	eFixed5.addComponent<TransformComponent>(Vector3(-10, 40, 0));
	eFixed5.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed5.addComponent<SphereComponent>(1);

	auto eLowerPoint5 = world.createEntity();
	eLowerPoint5.addComponent<TransformComponent>(Vector3(-10, 30, 0));
	eLowerPoint5.addComponent<SphereComponent>(1);
	eLowerPoint5.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable5 = world.createEntity();
	eCable5.addComponent<CableComponent>(eFixed5, eLowerPoint5, 10);


	// ------------------------------------------------------------------------
	auto eFixed6 = world.createEntity();
	eFixed6.addComponent<TransformComponent>(Vector3(-20, 40, 0));
	eFixed6.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed6.addComponent<SphereComponent>(1);

	auto eLowerPoint6 = world.createEntity();
	eLowerPoint6.addComponent<TransformComponent>(Vector3(-20, 30, 0));
	eLowerPoint6.addComponent<SphereComponent>(1);
	eLowerPoint6.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable6 = world.createEntity();
	eCable6.addComponent<CableComponent>(eFixed6, eLowerPoint6, 10);

	// ------------------------------------------------------------------------
	/*
	auto eFixed7 = world.createEntity();
	eFixed7.addComponent<TransformComponent>(Vector3(-30, 40, 0));
	eFixed7.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed7.addComponent<SphereComponent>(1);

	auto eLowerPoint7 = world.createEntity();
	eLowerPoint7.addComponent<TransformComponent>(Vector3(-30, 30, 0));
	eLowerPoint7.addComponent<SphereComponent>(1);
	eLowerPoint7.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0); 

	auto eCable7 = world.createEntity();
	eCable7.addComponent<CableComponent>(eFixed7, eLowerPoint7, 10);
	*/
	// ------------------------------------------------------------------------
	/*
	auto eFixed8 = world.createEntity();
	eFixed8.addComponent<TransformComponent>(Vector3(-40, 40, 0));
	eFixed8.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed8.addComponent<SphereComponent>(1);

	auto eLowerPoint8 = world.createEntity();
	eLowerPoint8.addComponent<TransformComponent>(Vector3(-40, 30, 0));
	eLowerPoint8.addComponent<SphereComponent>(1);
	eLowerPoint8.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable8 = world.createEntity();
	eCable8.addComponent<CableComponent>(eFixed8, eLowerPoint8, 10);
	*/
	// ------------------------------------------------------------------------
	//auto eRod1 = world.createEntity();
	//eRod1.addComponent<RodComponent>(eFixed, eFixed2, 10);
	auto eRod2 = world.createEntity();
	eRod2.addComponent<RodComponent>(eFixed2, eFixed3, 10);
	auto eRod3 = world.createEntity();
	eRod3.addComponent<RodComponent>(eFixed3, eFixed4, 10);
	auto eRod4 = world.createEntity();
	eRod4.addComponent<RodComponent>(eFixed4, eFixed5, 10); 
	auto eRod5 = world.createEntity();
	eRod5.addComponent<RodComponent>(eFixed5, eFixed6, 10);
	//auto eRod6 = world.createEntity();
	//eRod6.addComponent<RodComponent>(eFixed6, eFixed7, 10);
	//auto eRod7 = world.createEntity();
	//eRod7.addComponent<RodComponent>(eFixed7, eFixed8, 10);




	// ------------------------------------------------------------------------
	/*
	auto eFixed11 = world.createEntity();
	eFixed11.addComponent<TransformComponent>(Vector3(30, 40, 10));
	eFixed11.addComponent<SphereComponent>(1);
	eFixed11.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	//eFixed.addComponent<ParticleComponent>(1, Vector3(0,0,0), 0);

	auto eLowerPoint11 = world.createEntity();
	eLowerPoint11.addComponent<TransformComponent>(Vector3(30, 30, 10));
	eLowerPoint11.addComponent<SphereComponent>(1);
	eLowerPoint11.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0); 

	auto eCable11 = world.createEntity();
	eCable11.addComponent<CableComponent>(eFixed11, eLowerPoint11, 10); 
	*/

	// ------------------------------------------------------------------------
	auto eFixed12 = world.createEntity();
	eFixed12.addComponent<TransformComponent>(Vector3(20, 40, 10));
	eFixed12.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed12.addComponent<SphereComponent>(1);

	auto eLowerPoint12 = world.createEntity();
	eLowerPoint12.addComponent<TransformComponent>(Vector3(20, 30, 10));
	eLowerPoint12.addComponent<SphereComponent>(1);
	eLowerPoint12.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable12 = world.createEntity();
	eCable12.addComponent<CableComponent>(eFixed12, eLowerPoint12, 10);

	// ------------------------------------------------------------------------
	auto eFixed13 = world.createEntity();
	eFixed13.addComponent<TransformComponent>(Vector3(10, 40, 10));
	eFixed13.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed13.addComponent<SphereComponent>(1);

	auto eLowerPoint13 = world.createEntity();
	eLowerPoint13.addComponent<TransformComponent>(Vector3(10, 30, 10));
	eLowerPoint13.addComponent<SphereComponent>(1);
	eLowerPoint13.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable13 = world.createEntity();
	eCable13.addComponent<CableComponent>(eFixed13, eLowerPoint13, 10);

	// ------------------------------------------------------------------------
	auto eFixed14 = world.createEntity();
	eFixed14.addComponent<TransformComponent>(Vector3(0, 40, 10));
	eFixed14.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed14.addComponent<SphereComponent>(1);

	auto eLowerPoint14 = world.createEntity();
	eLowerPoint14.addComponent<TransformComponent>(Vector3(0, 30, 10));
	eLowerPoint14.addComponent<SphereComponent>(1);
	eLowerPoint14.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable14 = world.createEntity();
	eCable14.addComponent<CableComponent>(eFixed14, eLowerPoint14, 10);

	// ------------------------------------------------------------------------
	auto eFixed15 = world.createEntity();
	eFixed15.addComponent<TransformComponent>(Vector3(-10, 40, 10));
	eFixed15.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed15.addComponent<SphereComponent>(1);

	auto eLowerPoint15 = world.createEntity();
	eLowerPoint15.addComponent<TransformComponent>(Vector3(-10, 30, 10));
	eLowerPoint15.addComponent<SphereComponent>(1);
	eLowerPoint15.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable15 = world.createEntity();
	eCable15.addComponent<CableComponent>(eFixed15, eLowerPoint15, 10);

	// ------------------------------------------------------------------------
	auto eFixed16 = world.createEntity();
	eFixed16.addComponent<TransformComponent>(Vector3(-20, 40, 10));
	eFixed16.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed16.addComponent<SphereComponent>(1);

	auto eLowerPoint16 = world.createEntity();
	eLowerPoint16.addComponent<TransformComponent>(Vector3(-20, 30, 10));
	eLowerPoint16.addComponent<SphereComponent>(1);
	eLowerPoint16.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable16 = world.createEntity();
	eCable16.addComponent<CableComponent>(eFixed16, eLowerPoint16, 10);

	// ------------------------------------------------------------------------
	/*
	auto eFixed17 = world.createEntity();
	eFixed17.addComponent<TransformComponent>(Vector3(-30, 40, 10));
	eFixed17.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed17.addComponent<SphereComponent>(1);

	auto eLowerPoint17 = world.createEntity();
	eLowerPoint17.addComponent<TransformComponent>(Vector3(-30, 30, 10));
	eLowerPoint17.addComponent<SphereComponent>(1);
	eLowerPoint17.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0); 

	auto eCable17 = world.createEntity();
	eCable17.addComponent<CableComponent>(eFixed17, eLowerPoint17, 10); 
	*/
	// ------------------------------------------------------------------------
	/*
	auto eFixed18 = world.createEntity();
	eFixed18.addComponent<TransformComponent>(Vector3(-40, 40, 10));
	eFixed18.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed18.addComponent<SphereComponent>(1); 

	auto eLowerPoint18 = world.createEntity();
	eLowerPoint18.addComponent<TransformComponent>(Vector3(-40, 30, 10));
	eLowerPoint18.addComponent<SphereComponent>(1);
	eLowerPoint18.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable18 = world.createEntity();
	eCable18.addComponent<CableComponent>(eFixed18, eLowerPoint18, 10);
	*/
	// -----------------------------------------------------------------------------------------

	//auto eRod11 = world.createEntity();
	//eRod11.addComponent<RodComponent>(eFixed11, eFixed12, 10);
	auto eRod12 = world.createEntity(); 
	eRod12.addComponent<RodComponent>(eFixed12, eFixed13, 10); 
	auto eRod13 = world.createEntity();
	eRod13.addComponent<RodComponent>(eFixed13, eFixed14, 10);
	auto eRod14 = world.createEntity();
	eRod14.addComponent<RodComponent>(eFixed14, eFixed15, 10);
	auto eRod15 = world.createEntity();
	eRod15.addComponent<RodComponent>(eFixed15, eFixed16, 10);
	//auto eRod16 = world.createEntity();
	//eRod16.addComponent<RodComponent>(eFixed16, eFixed17, 10);
	//auto eRod17 = world.createEntity();
	//eRod17.addComponent<RodComponent>(eFixed17, eFixed18, 10);

	
	
	


	/*
	auto eRodDiag1 = world.createEntity();
	eRodDiag1.addComponent<RodComponent>(e1, e3, 20);
	auto eRodDiag2 = world.createEntity();
	eRodDiag2.addComponent<RodComponent>(e2, e4, 20); 
	*/

}

void SetupLights(ECSWorld& world)
{
	auto l = world.createEntity();
	l.addComponent<TransformComponent>(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(-45, 180, 0));
	l.addComponent<DynamicDirectionalLightComponent>(Color(0.00, 0.0, 0), Color(0.0f, 0.1f, 0.2f), Color(0.0f, 0.1f, 0.2f));

	// Lanterns
	auto pl1 = world.createEntity();
	pl1.addComponent<TransformComponent>(Vector3(22, 14, 48.5f));
	pl1.addComponent<DynamicPointLightComponent>(100.0f, Color(0.1, 0, 0), Color(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f));
	pl1.addComponent<ParticleComponent>();
	auto hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(23, 15, 48.0f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl1);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22, 13.5f, 50.5f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl1);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(21, 12.5f, 47.5f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl1);

	auto pl2 = world.createEntity();
	pl2.addComponent<TransformComponent>(Vector3(-14.5f, 14, 49.0f));
	pl2.addComponent<DynamicPointLightComponent>(100.0f, Color(0, 0, 0.1f), Color(0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f));
	pl2.addComponent<ParticleComponent>();
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f + 1, 14 - 1, 49.0f - 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl2);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 0.5f, 14 + 1, 49.0f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl2);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f, 14 - 1, 49.0f + 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl2);
	
	auto pl3 = world.createEntity();
	pl3.addComponent<TransformComponent>(Vector3(22, 14, -62.0f));
	pl3.addComponent<DynamicPointLightComponent>(100.0f, Color(0, 0.1f, 0), Color(0.0f, 1.0f, 0.0f), Color(0.0f, 1.0f, 0.0f));
	pl3.addComponent<ParticleComponent>();
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22 - 1, 14 - 1, -62.0f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl3);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22, 14 + 0.5f, -62.0f - 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl3);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22 + 1, 14, -62.0f + 0.5f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl3);

	auto pl4 = world.createEntity();
	pl4.addComponent<TransformComponent>(Vector3(-14.5f, 14, -61.5f));
	pl4.addComponent<DynamicPointLightComponent>(100.0f, Color(0.1, 0.05, 0), Color(1.0f, 0.55f, 0.0f), Color(1.0f, 0.55f, 0.0f));
	pl4.addComponent<ParticleComponent>();
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 1, 14, -61.5f -1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl4);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 0.25f, 14 - 0.5f, -61.5f + 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl4);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f + 0.5f, 14+ 1, -61.5f + 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl4);

	// Spears
	std::vector<Color> cols = { Color(1,0,0), Color(0,1,0), Color(0,0,1), Color(0.7f,0.55f,0) };
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pl1 = world.createEntity();
			pl1.addComponent<TransformComponent>(Vector3((i % 2 == 0 ? 8 : -1), 85, 49.5f - 37 * j), Vector3(1, 1, 1), Vector3(180, 0, 0));
			pl1.addComponent<DynamicSpotLightComponent>(10.0f, 100, Color(0, 0, 0), cols[3 - j], cols[3 - j], 5);
			pl1.addComponent<RotateComponent>((i % 2 == 0 ? 1 : -1) * 100,100,100);
		}
	}
}