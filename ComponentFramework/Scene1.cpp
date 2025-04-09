#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene1.h"
#include "MMath.h"
#include "CameraActor.h"
#include "LightActor.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "MaterialComponent.h"
#include "AiComponent.h"
#include "LocationManager.h"
#include "Timing.h"

#include "ScottCollisionComponent.h"
#include "ScottCollisionSystem.h"

#include "CollisionSystem.h"

#include <chrono>
#include <thread>
#include "CollisionComponent.h"
#include <Quad.h>
#include <QuadMath.h>
// test test delete me 
using namespace MATH;
using namespace MATHEX;


Scene1::Scene1(SceneManager* manager) : sceneManager(manager), engine(nullptr), drawNormals(false), drawOverlay(false), 
tileHeight(0.0f), tileWidth(0.0f), xAxis(0.0f), yAxis(0.0f)  {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

bool Scene1::OnCreate() {
	//Timing timing("Scene1::OnCreate");

	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
	assetManager = std::make_shared<AssetManager>();
	orientationCam = QMath::angleAxisRotation(10.0f, Vec3(1.0f, 0.0f, 0.0f));
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -10.0f), orientationCam);
	camera->OnCreate();
	camera->GetProjectionMatrix().print("ProjectionMatrix");
	camera->GetViewMatrix().print("ViewMatrix");

	light = std::make_shared<LightActor>(nullptr, LightStyle::DirectionLight, Vec3(1.0f, 0.0f, 0.0f),Vec4(0.9f,0.9f,0.9f,0.0f));
	light->OnCreate();

	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	Ref<CollisionComponent> cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::Sphere, 1.0f);
	Ref<PhysicsComponent> pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(0.0f, 0.0f, 4.1f), orientationBoard);
	Ref<TransformComponent> tc = std::make_shared<TransformComponent>(nullptr, Vec3(-10.0f, -10.0f, 0.0f), orientationBoard);


	

	for (int i = 0; i < 3; i++) {
		float x = 0.0f - (i * 90.0f);
		bg = std::make_shared<Actor>(nullptr);
		orientationBg = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
		bg->AddComponent<TransformComponent>(nullptr, Vec3(x, 0.0f, -70.0f), orientationBoard, Vec3(15.0f, 15.0f, 1.0f));
		bg->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
		bg->AddComponent<ShaderComponent>(shader);
		bg->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("city"));
		AddActor(bg);
	}
	
	deathFloor = std::make_shared<Actor>(nullptr);
	//orientationBoard = QMath::angleAxisRotation(276.0f, Vec3(1.0f, 0.0f, 0.0f));
	tc = std::make_shared<TransformComponent>(deathFloor.get(), Vec3(0.0f, -40.0f, -8.0f), QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f)));
	cc = std::make_shared<CollisionComponent>(deathFloor.get(), ColliderType::PLANE, 0.0f, Vec3(0.0f, 1.0f, 0.0f), -20.0f);
	deathFloor->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	deathFloor->AddComponent<ShaderComponent>(shader);
	deathFloor->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));
	deathFloor->AddComponent(cc);
	deathFloor->AddComponent(tc);
	deathFloor->SetName("DeathFloor");
	deathFloor->OnCreate();
	AddActor(deathFloor);


	factory = std::make_shared<Actor>(nullptr);
	/*AddAABBCollisionBox(factory, Vec3(30.0f, 0.0f, -10.0f),
		QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.05, 0.05, 0.05));*/
	factory->AddComponent<TransformComponent>(factory.get(), Vec3(30.0f, 0.0f, -10.0f), QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.05, 0.05, 0.05));
	factory->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Factory"));
	factory->AddComponent<ShaderComponent>(shader);
	factory->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	//factory->AddComponent(tc);
	factory->SetName("Factory");
	factory->OnCreate();
	AddActor(factory);

	factoryCollisionBox = std::make_shared<Actor>(nullptr);
	Vec3 boxPosF = Vec3(0.0f, -5.0f, -10.0f); // location
	Vec3 boxScaleF = Vec3(30.0f, 1.0f, 15.0f); // cube size
	Quaternion boxRotF = QMath::angleAxisRotation(0.0f, Vec3(1, 0, 0));
	tc = std::make_shared<TransformComponent>(factoryCollisionBox.get(), boxPosF, boxRotF, boxScaleF);
	cc = std::make_shared<CollisionComponent>(factoryCollisionBox.get(), ColliderType::AABB);
	cc->SetAABB(boxPosF, boxRotF, boxScaleF.x / 2, boxScaleF.y / 2, boxScaleF.z / 2); // set AABB

	factoryCollisionBox->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Box"));
	factoryCollisionBox->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	factoryCollisionBox->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));

	factoryCollisionBox->AddComponent(tc);
	factoryCollisionBox->AddComponent(cc);
	factoryCollisionBox->SetName("FactoryCollisionBox");
	factoryCollisionBox->OnCreate();
	
	AddActor(factoryCollisionBox);


	/*AddAABBCollisionBox(factory, Vec3(30.0f, 0.0f, -10.0f),
		QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.05, 0.05, 0.05));*/
	//tc = std::make_shared<TransformComponent>(factory.get(), Vec3(30.0f, 0.0f, -10.0f), QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.05, 0.05, 0.05));



	for (int i = 0; i < 8; i++) {
		float x = -70.0f - (i * 17.0f);
		auto road = std::make_shared<Actor>(Bridge.get());

		road->AddComponent<TransformComponent>(nullptr, Vec3(x, 12.2f, -6.4f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(1.72, 1.0, 0.8));
		road->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
		road->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));
		road->AddComponent<ShaderComponent>(shader);
		road->OnCreate();
		AddActor(road);
	}
	
	/*Ref <Actor> Lava[2];

	Lava[0] = std::make_shared<Actor>(factory.get());

	Vec3 lavaPos = Vec3(-360.0f, 0.0f, 40.0f); 
	Quaternion lavaRot = QMath::angleAxisRotation(0.0f, Vec3(1, 0, 0));
	Vec3 lavaScale = Vec3(10.0f, 1.0f, 4.0f);

	tc = std::make_shared<TransformComponent>(nullptr, Vec3(-360.0f, 0.0f, 40.0f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f))
		* QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(30.0, 30.0, 30.0));
	cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::AABB);
	cc->SetAABB(lavaPos, lavaRot, lavaScale.x / 2, lavaScale.y / 2, lavaScale.z / 2);
	Lava[0]->AddComponent(cc);
	Lava[0]->AddComponent(tc);
	Lava[0]->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	Lava[0]->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("lavaTexture"));
	Lava[0]->AddComponent<ShaderComponent>(shader);

	Lava[0]->SetName("LAVA[0]");
	Lava[0]->OnCreate();
	AddActor(Lava[0]);
	collisionSystem.AddActor(Lava[0]);
	transformSystem.AddActor(Lava[0]);


	Vec3 lavaPos2 = Vec3(360.0f, 65.0f, 40.0f);
	Quaternion lavaRot2 = QMath::angleAxisRotation(0.0f, Vec3(1, 0, 0));
	Vec3 lavaScale2 = Vec3(10.0f, 1.0f, 4.0f);

	tc = std::make_shared<TransformComponent>(nullptr, Vec3(360.0f, 65.0f, 40.0f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f))
		* QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(30.0, 30.0, 30.0));
	cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::AABB);
	cc->SetAABB(lavaPos2, lavaRot2, lavaScale2.x / 2, lavaScale2.y / 2, lavaScale2.z / 2);
	Lava[1]->AddComponent(cc);
	Lava[1]->AddComponent(tc);
	Lava[1]->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	Lava[1]->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("lavaTexture"));
	Lava[1]->AddComponent<ShaderComponent>(shader);

	Lava[1]->SetName("LAVA[1]");
	Lava[1]->OnCreate();
	AddActor(Lava[1]);
	collisionSystem.AddActor(Lava[1]);
	transformSystem.AddActor(Lava[1]);*/

	Ref <Actor> Lava[2];

	Lava[0] = std::make_shared<Actor>(factory.get());
	Lava[0]->AddComponent<TransformComponent>(nullptr, Vec3(-360.0f, 0.0f, 40.0f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(30.0, 30.0, 30.0));
	Lava[0]->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	Lava[0]->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("lavaTexture"));
	Lava[0]->AddComponent<ShaderComponent>(shader);
	//Lava[0]->AddComponent(cc);

	Lava[0]->OnCreate();
	AddActor(Lava[0]);

	Lava[1] = std::make_shared<Actor>(factory.get());
	Lava[1]->AddComponent<TransformComponent>(nullptr, Vec3(360.0f, 65.0f, 40.0f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(30.0, 30.0, 30.0));
	Lava[1]->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	Lava[1]->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("lavaTexture"));
	Lava[1]->AddComponent<ShaderComponent>(shader);
	//Lava[1]->AddComponent(cc);

	Lava[1]->OnCreate();
	AddActor(Lava[1]);

	bill = std::make_shared<Actor>(Bridge.get());
	orientationBill = QMath::angleAxisRotation(1800.0f, Vec3(0.0f, 1.0f, 0.0f));
	bill->AddComponent<TransformComponent>(nullptr, Vec3(-182.0f, 11.0f, -15.0f), orientationBill, Vec3(0.3, 0.3, 0.3));
	bill->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Board"));
	bill->AddComponent<ShaderComponent>(shader);
	bill->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("billAds"));
	AddActor(bill);
	
	Vec3 boardPos =  actors[0]->GetComponent<TransformComponent>()->GetPosition();
	//std::cout << "GameBoard Position: ("
		/*<< boardPos.x << ", "
		<< boardPos.y << ", "
		<< boardPos.z << ")\n"*/;

		// box over lava 1
		TestCube = std::make_shared<Actor>(nullptr);

		Vec3 boxPos1 = Vec3(4.0f, 1.5f, -8.0f); 
		Vec3 boxScale1 = Vec3(1.0f, 1.0f, 1.0f); 
		Quaternion boxRot1 = QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f));


		tc = std::make_shared<TransformComponent>(TestCube.get(), boxPos1, boxRot1, boxScale1);
		cc = std::make_shared<CollisionComponent>(TestCube.get(), ColliderType::AABB);
		cc->SetAABB(boxPos1, boxRot1, boxScale1.x / 2, boxScale1.y / 2, boxScale1.z / 2); 

		TestCube->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Box"));
		TestCube->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WoodBox"));
		TestCube->AddComponent<ShaderComponent>(shader);

		TestCube->AddComponent(tc);
		TestCube->AddComponent(cc);
		TestCube->SetName("Box1");
		TestCube->OnCreate();
		AddActor(TestCube);

		collisionSystem.AddActor(TestCube);
		transformSystem.AddActor(TestCube);

		// box over lava 2
		TestCube1 = std::make_shared<Actor>(nullptr);
		Vec3 boxPos2 = Vec3(8.0f, 1.5f, -8.0f); 
		Vec3 boxScale2 = Vec3(1.0f, 1.0f, 1.0f); 
		Quaternion boxRot2 = QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f));
		tc = std::make_shared<TransformComponent>(TestCube1.get(), boxPos2, boxRot2, boxScale2);
		cc = std::make_shared<CollisionComponent>(TestCube1.get(), ColliderType::AABB);
		cc->SetAABB(boxPos2, boxRot2, boxScale2.x / 2, boxScale2.y / 2, boxScale2.z / 2); 

		TestCube1->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Box"));
		TestCube1->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WoodBox"));
		TestCube1->AddComponent<ShaderComponent>(shader);

		TestCube1->AddComponent(tc);
		TestCube1->AddComponent(cc);
		TestCube1->SetName("Box2");
		TestCube1->OnCreate();
		AddActor(TestCube1);

		collisionSystem.AddActor(TestCube1);
		transformSystem.AddActor(TestCube1);

	Barrel = std::make_shared<Actor>(nullptr);
	tc = std::make_shared<TransformComponent>(Barrel.get(), Vec3(0.0f, 20.0f, 15), orientationBill);
	Barrel->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Barrel"));
	Barrel->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BarrelDif"));
	Barrel->AddComponent<ShaderComponent>(shader);
	Barrel->AddComponent(tc);
	Barrel->OnCreate();
	AddActor(Barrel);


	hack = std::make_shared<Actor>(nullptr);
	Quaternion hackRotation = QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f));
	Vec3 hackPosition = Vec3(71.0f, 5.0f, -5.0f);     // Adjust position as needed
	Vec3 hackVelocity = Vec3(0.0f, 0.0f, 0.0f);         // Initial velocity
	Vec3 hackScale = Vec3(0.0f, 0.0f, 0.0f);            // Scale of the object

	// Add Physics Component instead of Transform
	hack->AddComponent<PhysicsComponent>(nullptr, hackPosition, hackRotation, hackVelocity, Vec3(), Vec3(), hackScale);

	// Optional: Add collision
	hack->AddComponent<CollisionComponent>(nullptr, ColliderType::Sphere, 10.0f);

	// Visuals
	hack->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Box"));
	hack->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WoodBox"));
	hack->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));

	hack->OnCreate();
	AddActor(hack);
	collisionSystem.AddActor(hack);
	physicsSystem.AddActor(hack);
	transformSystem.AddActor(hack);

	Ref<Actor> car = std::make_shared<Actor>(Bridge.get());

	Vec3 carPos = Vec3(21.7f, -2.0f, -7.0f);
	Quaternion carRot = orientationBill;
	Vec3 collisionScale = Vec3(4.0f, 1.0f, 2.0f);

	tc = std::make_shared<TransformComponent>(car.get(), Vec3(-130.0f, 13.5f, -5.0f), orientationBill, Vec3(7.0f, 7.0f, 7.0f));
	cc = std::make_shared<CollisionComponent>(car.get(), ColliderType::AABB);
	cc->SetAABB(carPos, carRot, collisionScale.x / 2, collisionScale.y / 2, collisionScale.z / 2);
	car->AddComponent(cc);
	car->AddComponent(tc);
	car->AddComponent(assetManager->GetComponent<MeshComponent>("Car"));
	car->AddComponent<ShaderComponent>(shader);
	car->AddComponent(assetManager->GetComponent<MaterialComponent>("CarText"));
	car->SetName("Car");
	car->OnCreate();
	AddActor(car);
	collisionSystem.AddActor(car);
	transformSystem.AddActor(car);



	//Ref<Actor> Car = std::make_shared<Actor>(Bridge.get());

	//tc = std::make_shared<TransformComponent>(nullptr, Vec3(-130.0f, 13.5f, -5.0f), orientationBill, Vec3(7.0f, 7.0f, 7.0f));
	////cc = std::make_shared<CollisionComponent>(Car.get(), ColliderType::Sphere, 2.0f);
	////Car->AddComponent(cc);
	//Car->AddComponent(tc);
	////Car->AddComponent<TransformComponent>(nullptr, Vec3(-130.0f, 18.7f, -5.0f), orientationBill, Vec3(7.0, 7.0, 7.0));
	//Car->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Car"));
	//Car->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("CarText"));
	//Car->AddComponent<ShaderComponent>(shader);
	//Car->OnCreate();
	//AddActor(Car);
	//

	auto Scaf = std::make_shared<Actor>(factory.get());

	Scaf->AddComponent<TransformComponent>(nullptr, Vec3(-3000.0f, 250.0f, -60.0f), QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(1.0f, 1.0f, 1.0f));
	Scaf->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Scaffolding"));
	Scaf->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WoodBox"));
	Scaf->AddComponent<ShaderComponent>(shader);
	//Scaf->AddComponent(cc);
	Scaf->OnCreate();
	AddActor(Scaf);

	for (int i = 0; i < 3; i++) {
		float z = -4.5f - (i * 2.0f);
		auto Blocker = std::make_shared<Actor>(Bridge.get());

		Blocker->AddComponent<TransformComponent>(nullptr, Vec3(-145.0f, 12.2f, z), QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.02f, 0.02f, 0.02f));
		Blocker->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Blocker"));
		Blocker->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BlockerText"));
		Blocker->AddComponent<ShaderComponent>(shader);
		//Blocker->AddComponent(cc);
		Blocker->OnCreate();
		AddActor(Blocker);
	}
	auto Tunnels = std::make_shared<Actor>(factory.get());

	Tunnels->AddComponent<TransformComponent>(nullptr, Vec3(-310.0f, 1170.0f, -540.0f), QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f))* QMath::angleAxisRotation(-60.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(4.0, 4.0, 4.0));
	Tunnels->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("UTunnel"));
	Tunnels->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	Tunnels->AddComponent<ShaderComponent>(shader);
	//Tunnels->AddComponent(cc);
	Tunnels->OnCreate();
	AddActor(Tunnels);

	Ref <Actor> UTunnels[3];
		
	UTunnels[0] = std::make_shared<Actor>(factory.get());
	UTunnels[0]->AddComponent<TransformComponent>(nullptr, Vec3(-860.0f, 90.0f, -170.0f), QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(2.0, 2.0, 2.0));
	UTunnels[0]->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("UTunnel"));
	UTunnels[0]->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	UTunnels[0]->AddComponent<ShaderComponent>(shader);
	//UTunnels[0]->AddComponent(cc);
	
	UTunnels[0]->OnCreate();
	AddActor(UTunnels[0]);

	UTunnels[1] = std::make_shared<Actor>(factory.get());
	UTunnels[1]->AddComponent<TransformComponent>(nullptr, Vec3(-1060.0f, 140.0f, -170.0f), QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(2.0, 2.0, 2.0));
	UTunnels[1]->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("UTunnel"));
	UTunnels[1]->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	UTunnels[1]->AddComponent<ShaderComponent>(shader);
//	UTunnels[1]->AddComponent(cc);

	UTunnels[1]->OnCreate();
	AddActor(UTunnels[1]);


	UTunnels[2] = std::make_shared<Actor>(factory.get());
	UTunnels[2]->AddComponent<TransformComponent>(nullptr, Vec3(-3320.0f, 260.0f, -170.0f), QMath::angleAxisRotation(270.0f, Vec3(0.0f, 1.0f, 0.0f)) * QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(2.0, 2.0, 2.0));
	UTunnels[2]->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("UTunnel"));
	UTunnels[2]->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	UTunnels[2]->AddComponent<ShaderComponent>(shader);
	//UTunnels[2]->AddComponent(cc);

	UTunnels[2]->OnCreate();
	AddActor(UTunnels[2]);

	/*for (int i = 0; i < 8; i++) {
		float x = -70.0f - (i * 17.0f);
			auto StreetWalls = std::make_shared<Actor>(Bridge.get());

			StreetWalls->AddComponent<TransformComponent>(nullptr, Vec3(x, 16.8f, -7.0f), QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.13, 0.06, 0.06));
			StreetWalls->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("StreetWall"));
			StreetWalls->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
			StreetWalls->AddComponent<ShaderComponent>(shader);
			StreetWalls->AddComponent(cc);

			StreetWalls->OnCreate();
			AddActor(StreetWalls);

		}*/


		for (int i = 0; i < 8; i++) {
			float x = -70.0f - (i * 17.0f);
				auto streetLight = std::make_shared<Actor>(Bridge.get());

				streetLight->AddComponent<TransformComponent>(nullptr, Vec3(x, 12.2f, -12.0f), QMath::angleAxisRotation(180.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(0.5, 0.5, 0.5));
				streetLight->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("StreetLight"));
				streetLight->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
				streetLight->AddComponent<ShaderComponent>(shader);
				//streetLight->AddComponent(cc);

				streetLight->OnCreate();
				AddActor(streetLight);

		}


		Bridge = std::make_shared<Actor>(nullptr);

		Vec3 boxPosB = Vec3(-129.0f, 8.0f, -8.0f);
		Vec3 boxScaleB = Vec3(125.0f, 10.0f, 4.0f); 
		Quaternion boxRotB = QMath::angleAxisRotation(0.0f, Vec3(1, 0, 0));

		tc = std::make_shared<TransformComponent>(Bridge.get(), Vec3(-129.0, 5.66f, -8.0f), QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.20, 0.20, 0.20));
		cc = std::make_shared<CollisionComponent>(Bridge.get(), ColliderType::AABB);
		cc->SetAABB(boxPosB, boxRotB, boxScaleB.x / 2, boxScaleB.y / 2, boxScaleB.z / 2); 
		Bridge->AddComponent(tc);
		Bridge->AddComponent(cc);
		
		Bridge->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Bridge2"));
		Bridge->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("stone"));
		Bridge->AddComponent<ShaderComponent>(shader);
		Bridge->SetName("Bridge");
		Bridge->OnCreate();
		AddActor(Bridge);
		collisionSystem.AddActor(Bridge);
		transformSystem.AddActor(Bridge);
	
		
	character = std::make_shared<Actor>(nullptr);
	Quaternion playersQuaternion;
	pc = std::make_shared<PhysicsComponent>(character.get(), Vec3(0.0f, 0.0f, -8.0f),
		playersQuaternion, Vec3(), Vec3(), Vec3(), Vec3(3.0f, 3.0f, 3.0f));
	cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::Sphere, 2.0f);
	character->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Square"));
	character->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("RoboGun"));
	character->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("Billboard"));
	character->AddComponent(cc);
	character->AddComponent(pc);
	character->SetName("Player");
	character->OnCreate();
	AddActor(character);

	collisionSystem.SetCharacter(character);
	collisionSystem.AddActor(character);
	physicsSystem.AddActor(character);
	
	/*projectile = std::make_shared<Actor>(character.get());
	projectile->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Cube"));
	projectile->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BulletSkin"));
	projectile->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));*/


	LoadEnemies();
	SpawnAmmoAt(Vec3(6.0f, 1.0f, -6.0f));
	SpawnAmmoAt(Vec3(-47.0f, 15.0f, -5.0f));
	SpawnAmmoAt(Vec3(-160.0f, 12.2f, -6.0f));


//	collisionSystem.AddActor(factory);
    collisionSystem.AddActor(factoryCollisionBox);
	//collisionSystem.AddActor(BridgeCollisionBox);
	
	collisionSystem.AddActor(deathFloor);
	//collisionSystem.AddActor(Bridge);

//	transformSystem.AddActor(factory);

	transformSystem.AddActor(factoryCollisionBox);
	//transformSystem.AddActor(BridgeCollisionBox);
	
	transformSystem.AddActor(deathFloor);
	//transformSystem.AddActor(Bridge);



	//PATHFINDING REALTED 
	// Create the grid and graph for pathfinding
	createTiles();
	calculateConnectionWeights();

	// Sample pathfinding: Start at (0,0) and go to (7,7) - change as needed
	std::vector<Node*> path = graph->findPath(sceneNodes[10], sceneNodes[63]);
	//FOR DEBUGGING - SHOWS PATH TAKEN FOR ABOVE
	//std::cout << "Calculated Path:\n";
	for (Node* node : path) {
		int j, i;
		i = node->getLabel() / tiles[0].size(); // divide by the number of coloumns
		j = node->getLabel() % tiles.size();    // get the reminder by the number of rows
		//std::cout << "Path Node: " << node->getLabel() << " | " << " Tile Index: (" << i << ", " << j << ") " << " | "
		//<< "Position: (" << tiles[i][j]->getPosition().x << ", " << tiles[i][j]->getPosition().y << ")\n";
		tiles[i][j]->setPathTile(true);	
	}

	//END DEBUGGING FOR PATH TAKEN
	// Placeholder: Need to hook up path to AI

	// DECISION TREE RELATED
	// Load the decision tree from XML
// Find the first enemy dynamically
	Actor* player = character.get(); // The player remains the same

	std::cout << "[LOG]: Building decision trees for all enemies...\n";

	for (auto& pair : enemyHealth) {
		Actor* enemy = pair.first;

		auto tree = TreeBuilder::buildTree("Scene1.xml", enemy, player);
		if (!tree) {
			std::cerr << "[ERROR]: Failed to build decision tree for an enemy\n";
			continue;
		}
		enemyDecisionTrees[enemy] = tree;
	}

	std::cout << "[LOG]: Decision trees built for " << enemyDecisionTrees.size() << " enemies.\n";

	// Test decision tree evaluation
	/*if (decisionTreeRoot) {
		DecisionTreeNode* result = decisionTreeRoot->makeDecision(deltaTime);
		if (auto* action = dynamic_cast<Action*>(result)) {
			std::cout << "[LOG]: Decision Tree Result: " << action->GetActionName() << std::endl;
		}
		else {
			std::cerr << "[ERROR]: Root node evaluation failed" << std::endl;
		}
	}*/
	
	char c;
	engine = createIrrKlangDevice();
	//engine->play2D("./Audio/SciFiBG.mp3");
	engine->setSoundVolume(0.1);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	//std::cout << "hi" << std::endl;


	return true;
}

Scene1::~Scene1() {
	Debug::Info("Deleted Scene1: ", __FILE__, __LINE__);
	OnDestroy();
}

void Scene1::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);

	//FOR PATHFINDING
	// Delete each tile and its associated node in the 2D tiles vector
	//for (auto& row : tiles) {
	//	for (Tile* tile : row) {
	//		delete tile->getNode();  // Delete the node associated with each tile
	//		delete tile;              // Delete the tile itself
	//	}
	//}
	//Delete the graph
	

	//engine->drop(); // delete engine
	return;
}

void Scene1::FireProjectile() {
	if (sceneManager->clipAmmo <= 0) {
		std::cout << "Out of bullets! Press R to reload.\n";
		return;
	}

	// Get the character's transform
	auto charTransform = character->GetComponent<TransformComponent>();
	if (!charTransform) return;
	Ref<CollisionComponent> cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::Sphere, 1.0f);
	Vec3 charPos = charTransform->GetPosition(); // Start position of the character

	projectile = std::make_shared<Actor>(nullptr);
	projectile->AddComponent<TransformComponent>(nullptr, charPos, Quaternion(), Vec3( 0.5,0.5,0.5));
	projectile->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Cube"));
	projectile->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BulletSkin"));
	projectile->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	projectile->AddComponent(cc);
	Vec3 initialVelocity;
	if (facing) {
		initialVelocity = Vec3(60.0f, 0.0f, 0.0f);
	}
	else {
		initialVelocity = Vec3(-60.0f, 0.0f, 0.0f);
	}

	projectile->AddComponent<PhysicsComponent>(nullptr, charPos, Quaternion(), initialVelocity, Vec3(0.0f, -9.81f, 0.0f));
	
	projectile->OnCreate();
	AddActor(projectile);
	collisionSystem.AddActor(projectile);
	projectiles.push_back(projectile);
	
	engine->play2D("./Audio/gunShot.wav");


}


void Scene1::Reload()
{
	int bulletsNeeded = sceneManager->clipSize - sceneManager->clipAmmo;
	if (sceneManager->totalAmmo > 0 && bulletsNeeded > 0) {
		int bulletsToReload = std::min(bulletsNeeded, sceneManager->totalAmmo);
		sceneManager->clipAmmo += bulletsToReload;
		sceneManager->totalAmmo -= bulletsToReload;
	}

}



void Scene1::HandleEvents(const SDL_Event& sdlEvent) {
	//Timing timing("Scene1::HandleEvents");

	static int objID = -1;
	static Vec2 currentMousePos;
	static Vec2	lastMousePos;
	static float flip = 1.0f;
	Ref<TransformComponent> cameraTC;
	Ref<PhysicsComponent> characterPC;
	Ref<TransformComponent> characterTC;
	Ref<TransformComponent> gameBoardTC;

	Vec3 rotatedDirection;
	Vec3 direction;
	Quaternion start;
	Quaternion end;

	Quaternion orientationU;
	Quaternion orientationL;
	Quaternion orientationR;
	Quaternion orientationD;


	/// Handle Camera movement 

	switch (sdlEvent.type) {
	case SDL_KEYDOWN:

		characterTC = character->GetComponent<TransformComponent>();
		characterPC = character->GetComponent<PhysicsComponent>();

		int newX = hackingPlayerPos.x;
		int newY = hackingPlayerPos.y;

		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			if (!hackingMode) {
				movingDown = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.5f, 0.0f));
			}
			if (hackingMode && hackingPlayerPos.y < hackingTiles.size() - 1) newY++;
			break;

		case SDL_SCANCODE_S:
			if (!hackingMode) {
				movingDown = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, -0.5f, 0.0f));
			}
			if (hackingMode && hackingPlayerPos.y > 0) newY--;
			
			break;

		case SDL_SCANCODE_A:
			
			if (!hackingMode) {
				facing = false;
				facingLeft = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(-0.25f, 0.0f, 0.0f));
			}
				if (hackingMode && hackingPlayerPos.x > 0) newX--;
			break;

		case SDL_SCANCODE_D:
			
			if (!hackingMode) {
				facing = true;
				facingRight = true;
				facingLeft = false;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.25f, 0.0f, 0.0f));
			}
				if (hackingMode && hackingPlayerPos.x < hackingTiles[0].size() - 1) newX++;

			break;

		case SDL_SCANCODE_SPACE:
			std::cout << "SPACEBAR PRESSED!\n";
			if (!hackingMode && isGrounded && !isJumping) {
				std::cout << "JUMP TRIGGERED!\n";

				// Set vertical velocity
				Vec3 vel = characterPC->getVel();
				vel.y = jumpVelocity; // Apply upward velocity
				characterPC->SetVelocity(vel);

				// Optional horizontal nudge
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.25f, 0.0f, 0.0f));

				// Set jump state
				isJumping = true;
				isGrounded = false;

				std::cout << "Jump initiated! Velocity: " << vel << std::endl;
			}
			else if (hackingMode) {

				if (hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->getNode()->getIsBlocked()) {
					hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->getNode()->setIsBlocked(false);

					redTilePositions.erase(
						std::remove(redTilePositions.begin(), redTilePositions.end(),
							std::make_pair(hackingPlayerPos.y, hackingPlayerPos.x)),
						
						redTilePositions.end()
					);
					engine->play2D("./Audio/beep.wav");

					if (redTilePositions.empty()) {
						SDL_Delay(1000);
						hackingMode = false;
						showHackingGrid = false;
						std::cout << "All red tiles cleared. Hacking mode off." << std::endl;
					}
				}


			}
			break;
		}

		if (hackingMode && (newX != hackingPlayerPos.x || newY != hackingPlayerPos.y)) {
			hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->setPathTile(false);
			hackingPlayerPos = Vec2(newX, newY);
			hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->setPathTile(true);
		}

		break;
	}

	switch (sdlEvent.type) {
	case SDL_KEYUP:
		//if (!hackingMode) {
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			//facing = false;
			movingUp = false;
			idleTexture = true;
			//index.y = 0.0f;
			break;
		case SDL_SCANCODE_S:
			//facing = false;
			movingDown = false;
			idleTexture = true;
			//index.y = 0.0f;
			break;
		case SDL_SCANCODE_A:
			//facing = false;
			facingLeft = false;
			idleTexture = true;
			//index.x = 0.0f;
			break;
		case SDL_SCANCODE_D:
			//facing = false;
			facingRight = false;
			idleTexture = true;
			//index.x = 0.0f;
			break;
		default:
			break;
		}
	}
		switch (sdlEvent.type) {


		case SDL_MOUSEBUTTONDOWN:
			if (!hackingMode) {
				if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
					if (sceneManager->clipAmmo > 0) {
						FireProjectile(), sceneManager->clipAmmo -= 1;
					}
				}
			}
			break;



		case SDL_KEYDOWN:
			cameraTC = camera->GetComponent<TransformComponent>();
			characterTC = character->GetComponent<PhysicsComponent>();
			//gameBoardTC = gameboard->GetComponent<TransformComponent>();


			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_LEFT:
				cameraTC->SetTransform(cameraTC->GetPosition() + Vec3(-0.1f, 0.0f, 0.0f), cameraTC->GetQuaternion());
				camera->UpdateViewMatrix();
				break;

			case  SDL_SCANCODE_RIGHT:
				cameraTC->SetTransform(cameraTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f), cameraTC->GetQuaternion());
				camera->UpdateViewMatrix();
				break;

			case SDL_SCANCODE_UP:
				cameraTC->SetTransform(cameraTC->GetPosition() + Vec3(0.0f, 0.0f, 0.1f), cameraTC->GetQuaternion());
				camera->UpdateViewMatrix();
				break;

			case SDL_SCANCODE_DOWN:
				cameraTC->SetTransform(cameraTC->GetPosition() + Vec3(0.0f, 0.0f, -0.1f), cameraTC->GetQuaternion());
				camera->UpdateViewMatrix();
				break;

			}



			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_E:
				cameraTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion() *
					QMath::angleAxisRotation(-2.0f, Vec3(0.0f, 1.0f, 0.0f)));
				//camera->UpdateViewMatrix();
				break;

			case SDL_SCANCODE_Q:
				cameraTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion() *
					QMath::angleAxisRotation(2.0f, Vec3(0.0f, 1.0f, 0.0f)));
				//camera->UpdateViewMatrix();
				break;

			case SDL_SCANCODE_Z:
				cameraTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion() *
					QMath::angleAxisRotation(2.0f, Vec3(1.0f, 0.0f, 0.0f)));
				//camera->UpdateViewMatrix();
				break;

			case SDL_SCANCODE_X:
				cameraTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion() *
					QMath::angleAxisRotation(-2.0f, Vec3(1.0f, 0.0f, 0.0f)));
				//camera->UpdateViewMatrix();
				break;


			case SDL_SCANCODE_B: {
				showTiles = !showTiles; // Toggle the visibility flag
			}
							   break;

			case SDL_SCANCODE_L:
				showHackingGrid = !showHackingGrid;
				hackingMode = showHackingGrid; // Toggle hacking mode
				if (showHackingGrid && hackingTiles.empty()) {
					createHackingGrid();
				}
				break;
			


			case SDL_SCANCODE_R:

				//characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.0f, -0.1f));
				//orientationR = QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn right
				//	QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
				//characterTC->SetTransform(characterTC->GetPosition(), characterTC->GetQuaternion());
				Reload();

				break;

			case SDL_SCANCODE_T:

				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.0f, 0.1f));
				orientationR = QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn right
					QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
				characterTC->SetTransform(characterTC->GetPosition(), characterTC->GetQuaternion());

				break;


			case SDL_SCANCODE_N:
				if (drawNormals == false) drawNormals = true;
				else drawNormals = false;
				break;

			case SDL_SCANCODE_O:
				if (drawOverlay == false) drawOverlay = true;
				else drawOverlay = false;
				break;

			default:
				break;
			}
			break;

		}


	}

void Scene1::Update(const float deltaTime) {
	//Timing timing("Scene1::Update");
	frameCount++;
	
	//START OF PREVIOUS SPRINT WORK
	//Ref<PhysicsComponent> characterTC;
	//Ref<PhysicsComponent> enemyTC;
	//Ref<PhysicsComponent> enemyTC2;

	//Vec3 mariosPos = character->GetComponent<PhysicsComponent>()->GetPosition();
	//Vec3 mariosVel = character->GetComponent<PhysicsComponent>()->getVel();

	//// Update the gameboard transform
	//gameboard->GetComponent<TransformComponent>()->Update(deltaTime);

	//// Get Mario's position (character's position)
	//locationManager.mariosPos = character->GetComponent<PhysicsComponent>()->GetPosition();

	//// --- Enemy 1 (actors[2]) Flee ---
	//Vec3 enemy1Pos = actors[2]->GetComponent<PhysicsComponent>()->GetPosition();
	//Vec3 enemy1Move = actors[2]->GetComponent<AiComponent>()->Flee(enemy1Pos, locationManager.mariosPos);

	//// Update Enemy 1's position
	//enemyTC = actors[2]->GetComponent<PhysicsComponent>();
	//enemyTC->SetTransform(enemyTC->GetPosition() + enemy1Move * deltaTime, enemyTC->GetQuaternion());

	//// --- Enemy 2 (actors[3]) Follow ---
	//Vec3 enemy2Vel = actors[3]->GetComponent<PhysicsComponent>()->getVel();
	//Vec3 enemy2Pos = actors[3]->GetComponent<PhysicsComponent>()->GetPosition();
	////Vec3 enemy2Move = actors[3]->GetComponent<AiComponent>()->Pursuit(enemy2Pos, mariosPos, enemy2Vel);
	//Vec3 enemy2Move = actors[3]->GetComponent<AiComponent>()->Arrive(enemy2Pos, mariosPos);

	//// Update Enemy 2's position
	//enemyTC2= actors[3]->GetComponent<PhysicsComponent>();
	//enemyTC2->SetTransform(enemyTC2->GetPosition() + enemy2Move * deltaTime, enemyTC2->GetQuaternion());

	// Debug or print Mario's position if needed
	// locationManager.mariosPos.print();
	// END OF PREVIOUS SPRINT WORK 

	//START OF SPRINT 3 WORK
	//DECISION TREE RELATED
	// Evaluate the Decision Tree
	// Update a single enemy using the decision tree

	// Player-hack collision detection
	
	if (footstepTimer > 0.0f) {
		footstepTimer -= deltaTime;
	}


	for (size_t i = 0; i < projectiles.size();) {
		auto projectile = projectiles[i];
		auto projTransform = projectile->GetComponent<TransformComponent>();
		auto projPhysics = projectile->GetComponent<PhysicsComponent>();
		auto projCollision = projectile->GetComponent<CollisionComponent>();

		if (!projTransform || !projPhysics || !projCollision) {
			projectiles.erase(projectiles.begin() + i);
			continue;
		}

		// Apply gravity and update position
		Vec3 newVel = projPhysics->getVel() + Vec3(0.0f, -9.8f * deltaTime, 0.0f);
		projPhysics->SetVelocity(newVel);
		projTransform->SetPosition(projTransform->GetPosition() + newVel * deltaTime);

		// Check for collision with enemies
		bool hitEnemy = false;
		for (auto& pair : enemyHealth) {
			Actor* enemy = pair.first;
			auto enemyTransform = enemy->GetComponent<TransformComponent>();
			auto enemyCollision = enemy->GetComponent<CollisionComponent>();
			if (!enemyTransform || !enemyCollision) continue;

			Sphere s1{ projTransform->GetPosition(), projCollision->GetRadius() };
			Sphere s2{ enemyTransform->GetPosition(), enemyCollision->GetRadius() };

			if (collisionSystem.SphereSphereCollisionDetection(s1, s2)) {
				enemyHealth[enemy] -= 50.0f; // Damage
				std::cout << "Enemy hit! Remaining health: " << enemyHealth[enemy] << std::endl;
				engine->play2D("./Audio/boom.wav");
				if (enemyHealth[enemy] <= 0.0f) {
					std::cout << "Enemy destroyed!" << std::endl;
					enemy->OnDestroy();
					enemyHealth.erase(enemy);
					actors.erase(std::remove_if(actors.begin(), actors.end(),
						[enemy](const Ref<Actor>& a) { return a.get() == enemy; }),
						actors.end());

				}
				projectile->OnDestroy(); 
				actors.erase(std::remove_if(actors.begin(), actors.end(),
					[&](const Ref<Actor>& a) { return a == projectile; }),
					actors.end());
				projectiles.erase(projectiles.begin() + i); 
				hitEnemy = true;
				break;
			}
		}

		if (!hitEnemy && projTransform->GetPosition().y > -50.0f) {
			++i;
		}
		else if (!hitEnemy) {
			projectiles.erase(projectiles.begin() + i);
		}
	}


	Ref<PhysicsComponent> playerPhysics = character->GetComponent<PhysicsComponent>();
	Vec3 playerPos = playerPhysics->GetPosition();

	// --- After projectile vs enemy logic ---

	Ref<CollisionComponent> playerCollision = character->GetComponent<CollisionComponent>();
	Ref<PhysicsComponent> hackPhysics = hack->GetComponent<PhysicsComponent>();
	Ref<CollisionComponent> hackCollision = hack->GetComponent<CollisionComponent>();

	if (!hackUsed && playerPhysics && playerCollision && hackPhysics && hackCollision) {
		Sphere playerSphere{ playerPhysics->GetPosition(), playerCollision->GetRadius() };
		Sphere hackSphere{ hackPhysics->GetPosition(), hackCollision->GetRadius() };

		if (collisionSystem.SphereSphereCollisionDetection(playerSphere, hackSphere)) {
			hackUsed = true;
			hackingMode = true;
			showHackingGrid = true;
			createHackingGrid(); // if not already done
			std::cout << "Player touched the hack object — Hacking mode ON\n";
		}
	}

	for (size_t i = 0; i < ammoPickups.size();) {
		auto& ammo = ammoPickups[i];
		auto ammoPhysics = ammo->GetComponent<PhysicsComponent>();
		auto ammoCollision = ammo->GetComponent<CollisionComponent>();

		if (!ammoPhysics || !ammoCollision || !playerPhysics || !playerCollision) {
			++i;
			continue;
		}

		Sphere playerSphere{ playerPhysics->GetPosition(), playerCollision->GetRadius() };
		Sphere ammoSphere{ ammoPhysics->GetPosition(), ammoCollision->GetRadius() };

		if (collisionSystem.SphereSphereCollisionDetection(playerSphere, ammoSphere)) {
			std::cout << "[INFO]: Ammo collected! +5 bullets.\n";
			sceneManager->totalAmmo += 5;
			engine->play2D("./Audio/ammoPickUp.wav");
			ammo->OnDestroy();

			// Remove from actors and ammo list
			actors.erase(std::remove_if(actors.begin(), actors.end(),
				[&](const Ref<Actor>& a) { return a == ammo; }), actors.end());

			ammoPickups.erase(ammoPickups.begin() + i);
		}
		else {
			++i;
		}
	}


	// Get the camera's transform
	Ref<TransformComponent> cameraTransform = camera->GetComponent<TransformComponent>();
	Vec3 cameraPos = cameraTransform->GetPosition();

	// Invert the X-axis movement
	cameraPos.x = -playerPos.x; // Invert player's X position for the camera
	cameraPos.y = -playerPos.y - 5.0f;

	// Apply the new position to the camera (Y and Z remain constant)
	cameraTransform->SetTransform(cameraPos, cameraTransform->GetQuaternion());

	// Update the camera's view matrix
	camera->UpdateViewMatrix();

	//const float gravity = -9.8f;        // Gravitational acceleration
	const float jumpStrength = 5.0f;   // Initial jump velocity
	const float moveSpeed = 30.0f;      // Movement speed
	static float verticalVelocity = 0.0f; // Character's vertical velocity

	//	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);
	auto characterTransform = character->GetComponent<PhysicsComponent>();
	//Ref<PhysicsComponent> playerPhysics = character->GetComponent<PhysicsComponent>();

	 // Gravity
	if (!isGrounded) {
		Vec3 currentVel = playerPhysics->getVel();
		currentVel.y += -0.0f * deltaTime;
		playerPhysics->SetVelocity(currentVel);
	}
	
	//isGrounded = false;

	if (isGrounded) {
		Vec3 vel = playerPhysics->getVel();
		if (isJumping) {
			vel.y = 0.0f;
			playerPhysics->SetVelocity(vel);
			isJumping = false;
			std::cout << "Landing. Jump ended.\n";
		}
	}


	// Handle horizontal motion (WASD input)
	Vec3 pos = characterTransform->GetPosition();
	Vec3 horizontalMove(0.0f, 0.0f, 0.0f); // Movement direction
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);
	if (!hackingMode) {
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D]) {
			if (footstepTimer <= 0.0f) {
				engine->play2D("./Audio/footStep.wav", false);
				footstepTimer = footstepCooldown;
			}
		}
	}
		//if (keystate[SDL_SCANCODE_W]) {
		//	movingUp = true;
		//	facing = true;
		//	horizontalMove.y += 1.0f; // Forward
		//}
		//else if (keystate[SDL_SCANCODE_S]) {
		//	movingDown = true;
		//	facing = true;
		//	horizontalMove.y -= 1.0f; // Backward
		//}
		//else if (keystate[SDL_SCANCODE_A]) {
		//	facingLeft = true; // last part for animation
		//	facing = true;
		//	horizontalMove.x -= 1.0f; // Left
		//}
		//else if (keystate[SDL_SCANCODE_D]) {
		//	facingRight = true;
		//	facing = true;
		//	horizontalMove.x += 1.0f; // Right
		//}
	//}
	// Normalize movement direction and scale by speed and deltaTime
	if (VMath::mag(horizontalMove) > 0.0f) {
		horizontalMove = VMath::normalize(horizontalMove) * moveSpeed * deltaTime;
	}

	// Combine horizontal and vertical motion
	pos += horizontalMove;        // Update horizontal position
	pos.z += verticalVelocity * deltaTime; // Update vertical position

	// Apply updated position to the character
	characterTransform->SetPosition(pos);


	Ref<PhysicsComponent>characterTC = character->GetComponent<PhysicsComponent>();
	Ref<PhysicsComponent> enemyTC;


	// Get Mario's position (character's position)
	Vec3 mariosPos = character->GetComponent<PhysicsComponent>()->GetPosition();
	locationManager.mariosPos = mariosPos;

	// Enemy 1 (actors[2]) Decision Tree Logic 
	auto& enemy = actors[2];

	// Update Enemy 1's state
	enemy->Update(deltaTime); // THIS DOES NOTHING 

	// Evaluate the decision tree
	for (auto& pair : enemyDecisionTrees) {
		Actor* enemy = pair.first;

		// Skip dead enemies
		if (enemyHealth.find(enemy) == enemyHealth.end() || enemyHealth[enemy] <= 0.0f) continue;

		DecisionTreeNode* tree = pair.second;
		enemy->Update(deltaTime);

		if (tree) {
			DecisionTreeNode* result = tree->makeDecision(deltaTime);
			if (auto* action = dynamic_cast<Action*>(result)) {
				action->makeDecision(deltaTime);

				if (action->GetActionName() == "Attack Player") {
					engine->play2D("./Audio/enemyAttack.wav");
					sceneManager->playerHealth -= 0.1f;
					std::cout << "[SCENE1]: Player took damage! Health is now " << sceneManager->playerHealth << "\n";
				}
			}
		}
	}

	if (!loadingStarted && sceneManager->playerHealth <= 0) {
		sceneManager->dead = true;
		loadingStarted = true;

		SpawnLoadingScreen();

		// Move and rotate the camera to face the loading screen
		if (camera) {
			auto camTransform = camera->GetComponent<TransformComponent>();
			if (camTransform) {
				Vec3 newCamPos = Vec3(0.0f, -18.0f, 0.0f); // match Scene3 position
				Quaternion newCamRot = QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f));
					

				camTransform->SetTransform(newCamPos, newCamRot);
				camera->UpdateViewMatrix();
			}
		}
	}



	character->Update(deltaTime);
	//collisionSystem.Update(deltaTime);
//	physicsSystem.Update(deltaTime);
	transformSystem.Update(deltaTime);

	// animation movement
	if (frameCount % 2 == 0) {
		if (idleTexture) {
			index.x = 0.0f;
			index.y = 0.0f;
		}
		else if (facingRight) {
			// Update x component for horizontal frames
			index.y = 0.0f;
			index.x += 1.0f;
			if (index.x > 7.0f) {
				index.x = 0.0f;
			}
			index.y = 0.0f;
		}
		else if (facingLeft) {
			index.y = 0.0f;
			index.x += 1.0f;
			if (index.x > 7.0f) {
				index.x = 0.0f;
			}
		}
		else if (movingUp) {
			index.y = 1.0f; // Update y component for vertical frames
			index.x += 1.0f;
			if (index.x > 7.0f) {
				index.x = 0.0f;
			}
	
		}
		else if (movingDown) {
			index.y = 1.0f; // Update y component for vertical frames
			index.x += 1.0f;
			if (index.x > 7.0f) {
				index.x = 0.0f;
			}
		}
		std::cout << index.x << ',' << index.y << ',' << frameCount << std::endl;
	}

	
	//std::cout << "Checking collision for character at: "
		//<< character->GetComponent<PhysicsComponent>()->GetPosition() << std::endl;

	// Update the gameboard transform
//	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);

	// Run all collisions
	collisionSystem.Update(deltaTime);
	physicsSystem.Update(deltaTime);
}



void Scene1::Render() const {
	//Timing timing("Scene1::Render");

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS); // JUST ADDED FOR TESTING ROTATION
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatriciesID());
	glBindBuffer(GL_UNIFORM_BUFFER, light->GetLightID());

	for (auto actor : actors) {
		glUseProgram(actor->GetComponent<ShaderComponent>()->GetProgram());
		glUniformMatrix4fv(actor->GetComponent<ShaderComponent>()->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
		glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
		glUniform2f(actor->GetComponent<ShaderComponent>()->GetUniformID("index"), index.x, index.y);
		
		
		
		actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);

	}

	glUseProgram(0);  // Unbind 

	// ** Render Collision Boxes (Wireframe Mode) **
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0f, 0.0f, 0.0f); 

	for (auto actor : actors) {
		auto collider = actor->GetComponent<CollisionComponent>();
		if (collider) {
			collider->Render();
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Reset polygon mode

	
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);  // Ensure fixed-function pipeline for tiles

	

	if (showHackingGrid) {

		for (const auto& row : hackingTiles) {
			for (Tile* tile : row) {
				tile->render();
			}
		}
	}

	if (showTiles) {
		for (const auto& row : tiles) {
			for (Tile* tile : row) {
				tile->render();
			}
		}
	}

	// ** Render Overlays **
	if (drawOverlay) {
		DrawMeshOverlay(Vec4(1.0f, 1.0f, 1.0f, 0.5f));
	}

	if (drawNormals) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.01f));
	}

	// ** Render Projectiles **
	for (const auto& projectile : projectiles) {
		projectile->Render();
	}

	// Reset OpenGL state after rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}



void Scene1::DrawMeshOverlay(const Vec4 color) const {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatriciesID());
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("DefaultShader");
	glUseProgram(shader->GetProgram());
	glUniform4fv(shader->GetUniformID("color"), 1, color);
	for (auto actor : actors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
		actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
	}
	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
	

void Scene1::DrawNormals(const Vec4 color) const {
	glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatriciesID());
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("DrawNormalsShader");
	glUseProgram(shader->GetProgram());
	glUniform4fv(shader->GetUniformID("color"), 1, color);
	for (auto actor : actors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
		actor->GetComponent<MeshComponent>()->Render();
	}
	glUseProgram(0);
}

void Scene1::SpawnAmmoAt(const Vec3& position) {
	auto ammo = std::make_shared<Actor>(nullptr);

	Quaternion rotation = QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f));
	Vec3 velocity = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);

	ammo->AddComponent<PhysicsComponent>(nullptr, position, rotation, velocity, Vec3(), Vec3(), scale);
	ammo->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Ammo"));
	ammo->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("AmmoDif"));
	ammo->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	ammo->AddComponent<CollisionComponent>(ammo.get(), ColliderType::Sphere, 8.0f);

	ammo->OnCreate();
	AddActor(ammo);
	collisionSystem.AddActor(ammo);
	physicsSystem.AddActor(ammo);
	ammoPickups.push_back(ammo);

}



void Scene1::LoadEnemies() {
	Ref<MeshComponent> mesh = assetManager->GetComponent<MeshComponent>("Plane");
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("Billboard");
	Ref<MaterialComponent> material = assetManager->GetComponent<MaterialComponent>("Enemy");

	// === Enemy 1 ===
	Ref<Actor> enemy1 = std::make_shared<Actor>(nullptr);
	enemy1->AddComponent<MeshComponent>(mesh);
	enemy1->AddComponent<ShaderComponent>(shader);
	enemy1->AddComponent<MaterialComponent>(material);
	enemy1->AddComponent<AiComponent>(enemy1.get());
	enemy1->AddComponent<CollisionComponent>(enemy1.get(), ColliderType::Sphere, 1.0f);
	enemy1->AddComponent<PhysicsComponent>(
		enemy1.get(),
		Vec3(0.0f, 0.0f, -10.0f), // position
		QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)) *
		QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) *
		QMath::angleAxisRotation(180.0f, Vec3(1.0f, 0.0f, 0.0f)), // rotation
		Vec3(0.0f,0.0f,0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), // velocity, accel, forces
		Vec3(5.15f, 5.15f, 5.15f)); // scale

	enemy1->OnCreate();
	AddActor(enemy1);
	collisionSystem.AddActor(enemy1);
	physicsSystem.AddActor(enemy1);
	transformSystem.AddActor(enemy1);
	enemyHealth[enemy1.get()] = 100.0f;

	// === Enemy 2 ===
	Ref<Actor> enemy2 = std::make_shared<Actor>(nullptr);
	enemy2->AddComponent<MeshComponent>(mesh);
	enemy2->AddComponent<ShaderComponent>(shader);
	enemy2->AddComponent<MaterialComponent>(material);
	enemy2->AddComponent<AiComponent>(enemy2.get());
	enemy2->AddComponent<CollisionComponent>(enemy2.get(), ColliderType::Sphere, 1.0f);
	enemy2->AddComponent<PhysicsComponent>(
		enemy2.get(),
		Vec3(10.0f, 0.0f, -10.0f), // different position
		QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)) *
		QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) *
		QMath::angleAxisRotation(180.0f, Vec3(1.0f, 0.0f, 0.0f)), // same upright rotation
		Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f),
		Vec3(0.75f, 0.75f, 0.75f));

	enemy2->OnCreate();
	AddActor(enemy2);
	collisionSystem.AddActor(enemy2);
	physicsSystem.AddActor(enemy2);
	transformSystem.AddActor(enemy2);
	enemyHealth[enemy2.get()] = 100.0f;
}


// Creates an 8x8 grid of tiles and initializes nodes
void Scene1::createTiles() {
	int gridSize = 8;

	//changed amounts to make fit with the rotation. 
	float tileWidth = 0.50f;
	float tileHeight = 0.28f;

	// Calculate aspect ratio as a float
	float aspectRatio = 1280.0f / 720.0f;

	if (aspectRatio > 1.0f) {
		tileHeight *= aspectRatio;
	}
	else {
		tileWidth /= aspectRatio;
	}

	// Center offset for the grid, accounting for adjusted tile dimensions. Changed for tile rotation
	float gridCenterX = (gridSize * tileWidth) / 2.0f;
	float gridCenterY = (gridSize * tileHeight) / 2.8f;
	float gridCenterZ = -5.0f; // Starting z-position 

	graph = new Graph();
	tiles.resize(gridSize, std::vector<Tile*>(gridSize));
	sceneNodes.resize(gridSize * gridSize);

	std::vector<std::pair<int, int>> blockedPositions = {
		{2, 3}, {4, 5}, {1, 1}, {3, 2}, {6, 4}, {6, 5}, {6, 6}, {6, 7}
	};

	int label = 0;

	// Create a rotation matrix for -45 degrees on the x-axis
	Matrix4 rotationMatrix = MMath::rotate(-45.0f, 1.0f, 0.0f, 0.0f);
	Matrix4 projectionMatrix = camera->GetProjectionMatrix();

	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			Node* node = new Node(label);
			sceneNodes[label] = node;


			Vec3 tilePos = Vec3((j * tileWidth) - gridCenterX, (i * tileHeight) - gridCenterY, gridCenterZ);

			// Additional offset based on row index `i`
			// Adjust this factor as needed to minimize gaps due to rotation
			float rowOffsetx = -0.2f * i; //not needed as its rotating along the x axis
			float rowOffsetY = -0.30f * tileHeight * i;
			float rowOffsetz = -0.68f * tileWidth * i;
			
			Matrix4 OffsetMatrix = MMath::translate(0.0f, rowOffsetY, rowOffsetz);
			
			Matrix4 translationMatrix = MMath::translate(tilePos);
			
			Tile* tile = new Tile(node, tilePos, tileWidth, tileHeight, this);
			tile->modelMatrix = projectionMatrix  * translationMatrix * OffsetMatrix  * rotationMatrix;

			tiles[i][j] = tile;

			if (std::find(blockedPositions.begin(), blockedPositions.end(), std::make_pair(i, j)) != blockedPositions.end()) {
				node->setIsBlocked(true);
			}

			label++;
		}
	}

	tiles[gridSize - 1][gridSize - 1]->setDestinationTile(true);

	if (!graph->OnCreate(sceneNodes)) {
		std::cerr << "Failed to initialize graph." << std::endl;
	}
}




// Sets up connections (left, right, up, down) between adjacent nodes
void Scene1::calculateConnectionWeights() {
	int gridSize = 8;

	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			Node* from = tiles[i][j]->getNode();

			// Connect left
			if (j > 0) {
				Node* to = tiles[i][j - 1]->getNode();
				graph->addWeightedConnection(from, to, 1.0f);
			}

			// Connect right
			if (j < gridSize - 1) {
				Node* to = tiles[i][j + 1]->getNode();
				graph->addWeightedConnection(from, to, 1.0f);
			}

			// Connect up
			if (i > 0) {
				Node* to = tiles[i - 1][j]->getNode();
				graph->addWeightedConnection(from, to, 1.0f);
			}

			// Connect down
			if (i < gridSize - 1) {
				Node* to = tiles[i + 1][j]->getNode();
				graph->addWeightedConnection(from, to, 1.0f);
			}
		}
	}
}



void Scene1::createHackingGrid() {
	int gridSize = 6; // Grid size
	float tileWidth = 0.50f;
	float tileHeight = 0.28f;

	// Adjust for aspect ratio
	float aspectRatio = 1280.0f / 720.0f;
	if (aspectRatio > 1.0f) {
		tileHeight *= aspectRatio;
	}
	else {
		tileWidth /= aspectRatio;
	}

	// Center offset for positioning
	float gridCenterX = (gridSize * tileWidth) / 2.0f;
	float gridCenterY = (gridSize * tileHeight) / 2.8f;
	float gridCenterZ = -5.0f;

	// Rotation and Projection
	Matrix4 rotationMatrix = MMath::rotate(0.0f, 1.0f, 0.0f, 0.0f);
	Matrix4 projectionMatrix = camera->GetProjectionMatrix();

	hackingTiles.clear();
	hackingTiles.resize(gridSize, std::vector<Tile*>(gridSize));

	
	int numRedTiles = (gridSize * gridSize) / 2; // 25% of tiles will be red
	srand(time(nullptr)); // Seed for randomness

	// Randomly select red tile positions
	while (redTilePositions.size() < numRedTiles) {
		int x = rand() % gridSize;
		int y = rand() % gridSize;
		if (std::find(redTilePositions.begin(), redTilePositions.end(), std::make_pair(x, y)) == redTilePositions.end()) {
			redTilePositions.push_back({ x, y });
		}
	}

	int label = 0;
	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			Node* node = new Node(label);
			sceneNodes[label] = node;

			Vec3 tilePos = Vec3(
				(j * tileWidth) - gridCenterX,
				(i * tileHeight) - gridCenterY,
				gridCenterZ
			);

			Matrix4 translationMatrix = MMath::translate(tilePos);
			Matrix4 modelMatrix = projectionMatrix * translationMatrix * rotationMatrix;

			Tile* tile = new Tile(node, tilePos, tileWidth, tileHeight, this);
			tile->modelMatrix = modelMatrix;

			// Set red tiles as "blocked"
			if (std::find(redTilePositions.begin(), redTilePositions.end(), std::make_pair(i, j)) != redTilePositions.end()) {
				node->setIsBlocked(true);
			}


			hackingTiles[i][j] = tile;
			label++;
		}
	}

	// Place the player cube at the starting position (Top Left: 0,0)
	hackingPlayerPos = Vec2(0, 0);
	hackingTiles[0][0]->setPathTile(true); // Highlight the starting tile
}


int Scene1::Pick(int x, int y) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); /// Paint the backgound white which is 0x00FFFFFF
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatriciesID());
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("ColorPickingShader");
	glUseProgram(shader->GetProgram());
	for (GLuint i = 0; i < actors.size(); i++) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, actors[i]->GetModelMatrix());
		glUniform1ui(shader->GetUniformID("colorID"), i);
		actors[i]->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
	}
	glUseProgram(0);
	GLuint colorIndex;
	glReadPixels(x, viewport.height-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &colorIndex);
	colorIndex &= 0x00FFFFFF; /// This zeros out the alpha component

	if (colorIndex == 0x00FFFFFF) return -1; /// Picked nothing
	else return colorIndex;
}

void Scene1::SpawnLoadingScreen() {
	loadingScreenActor = std::make_shared<Actor>(nullptr);

	// Set position and rotation
	Vec3 screenPos = Vec3(0.0f, 18.0f, -17.0f);;

	// Rotate plane to face along +X (normal = +X)
	Quaternion screenRotation = QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)) * QMath::angleAxisRotation(0.0f, Vec3(0.0f, 0.0f, 1.0f));

	loadingScreenActor->AddComponent<TransformComponent>(nullptr, screenPos, screenRotation, Vec3(3.0f, 2.0f, 4.0f));
	loadingScreenActor->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	loadingScreenActor->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	loadingScreenActor->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("DieLoading"));
	loadingScreenActor->SetName("DieLoading");
	loadingScreenActor->OnCreate();
	AddActor(loadingScreenActor);
}

