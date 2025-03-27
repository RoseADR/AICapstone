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


Scene1::Scene1(SceneManager* manager) : sceneManager(manager), engine(nullptr), drawNormals(false), drawOverlay(false) {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
}


bool Scene1::OnCreate() {
	// testing new math stuff
	Vec3 pos(-1, 0.5, 0); // This would be the position of your character. I've given an example

	// Fill in the corners of the platform in the quad constructor
	// Wind the quad anti-clockwise
	// Here's an example of a strange looking quad
	MATHEX::Quad quad = MATHEX::Quad(
		Vec3(0, 0, 0),
		Vec3(1, 0, 0),
		Vec3(0.5, 1, 0),
		Vec3(0, 1, 0)
	);

	// Find the closest point between the character's centre position and the quad
	// In this example it should return the vector (0, 0.5, 0) I think
	Vec3 closestPoint = MATHEX::QuadMath::closestPointOnQuad(pos, quad);
	//Timing timing("Scene1::OnCreate");

	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
	assetManager = std::make_shared<AssetManager>();
	orientationCam = QMath::angleAxisRotation(5.0f, Vec3(1.0f, 0.0f, 0.0f));
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -7.0f, -13.0f), orientationCam);
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
	//gameboard = std::make_shared<Actor>(nullptr);

	orientationBoard = QMath::angleAxisRotation(276.0f, Vec3(1.0f, 0.0f, 0.0f));
	//tc = std::make_shared<TransformComponent>(nullptr, Vec3(0.0f, -10.0f, -10.0f), orientationBoard);
	//cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::PLANE, 0.0f, Vec3(0.0f, -1.0f, 0.0f));
 //  // gameboard->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -10.0f, -10.0f), orientationBoard);
	//gameboard->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	//gameboard->AddComponent<ShaderComponent>(shader);
	//gameboard->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));
	//gameboard->AddComponent(cc);
	//gameboard->AddComponent(tc);
	//// <CollisionComponent>(nullptr,
	////	ColliderType::Sphere, Vec3(9.0f, 2.0f, 22.5f), // Width, height, depth
	////	0.0f, Vec3(-5.0f, 0.0f, 0.0f)); // Offset
	//// 
	//gameboard->OnCreate();
	//AddActor(gameboard);


	factory = std::make_shared<Actor>(nullptr);

	tc = std::make_shared<TransformComponent>(factory.get(), Vec3(30.0f, -5.0f, -10.0f), QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.05, 0.05, 0.05));
	cc = std::make_shared<CollisionComponent>(factory.get(), ColliderType::PLANE, 0.0f, Vec3(0.0f, 1.0f, 0.0f));
	factory->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Factory"));
	factory->AddComponent<ShaderComponent>(shader);
	factory->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	factory->AddComponent(cc);
	factory->AddComponent(tc);
	collisionSystem.SetFactory(factory);
	factory->OnCreate();
	AddActor(factory);

	//factory->ListComponents();


	for (int i = 0; i < 8; i++) {
		float x = -70.0f - (i * 17.0f);
		auto road = std::make_shared<Actor>(Bridge.get());

		road->AddComponent<TransformComponent>(nullptr, Vec3(x, 12.2f, -6.4f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(1.72, 1.0, 0.8));
		road->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
		road->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));
		road->AddComponent<ShaderComponent>(shader);
		//road->AddComponent(cc);

		road->OnCreate();
		AddActor(road);
	}
	
	Ref <Actor> Lava[2];

	Lava[0] = std::make_shared<Actor>(factory.get());
	Lava[0]->AddComponent<TransformComponent>(nullptr,Vec3(-360.0f, 0.0f, 40.0f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(30.0, 30.0, 30.0));
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
		for (int i = 0; i < 2; i++) {
			float x = -360.0f - (i * -90.0f);
			TestCube = std::make_shared<Actor>(factory.get());
			TestCube->AddComponent<TransformComponent>(nullptr, Vec3(x, 30.0f, 15.0f), orientationBill, Vec3(20.0, 20.0, 20.0));
			TestCube->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Box"));
			TestCube->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WoodBox"));
			TestCube->AddComponent<ShaderComponent>(shader);
			//TestCube->AddComponent(cc);
			TestCube->OnCreate();
			AddActor(TestCube);
		}
	 Barrel = std::make_shared<Actor>(factory.get());

//	Barrel->AddComponent<TransformComponent>(nullptr, Vec3(-600.0f, 20.0f, 200.0f), orientationBill, Vec3(13.0f, 13.0f, 13.0f));
	pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(-600.0f, -5.0f, -10.0f), orientationBill, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), 
		Vec3(0.0f, 0.0f, 0.0f), Vec3(13.0f, 13.0f, 13.0f));
	cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::Sphere, 2.0f); 
	Barrel->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Barrel"));
	Barrel->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BarrelDif"));
	Barrel->AddComponent<ShaderComponent>(shader);
	Barrel->AddComponent(pc);
	Barrel->AddComponent(cc);
	Barrel->OnCreate();
	AddActor(Barrel);

	Ref<Actor> Car = std::make_shared<Actor>(Bridge.get());

	tc = std::make_shared<TransformComponent>(nullptr, Vec3(-130.0f, 13.5f, -5.0f), orientationBill, Vec3(7.0f, 7.0f, 7.0f));
	//cc = std::make_shared<CollisionComponent>(Car.get(), ColliderType::Sphere, 2.0f);
	//Car->AddComponent(cc);
	Car->AddComponent(tc);
	//Car->AddComponent<TransformComponent>(nullptr, Vec3(-130.0f, 18.7f, -5.0f), orientationBill, Vec3(7.0, 7.0, 7.0));
	Car->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Car"));
	Car->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("CarText"));
	Car->AddComponent<ShaderComponent>(shader);
	Car->OnCreate();
	AddActor(Car);


	
	auto Scaf = std::make_shared<Actor>(factory.get());

	Scaf->AddComponent<TransformComponent>(nullptr, Vec3(-3000.0f, 340.0f, -60.0f), QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(1.0f, 1.0f, 1.0f));
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

	Tunnels->AddComponent<TransformComponent>(nullptr, Vec3(-860.0f, 50.0f, -170.0f), QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(2.0, 2.0, 2.0));
	Tunnels->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Tunnels"));
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
	UTunnels[2]->AddComponent<TransformComponent>(nullptr, Vec3(-3320.0f, 300.0f, -170.0f), QMath::angleAxisRotation(270.0f, Vec3(0.0f, 1.0f, 0.0f)) * QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(2.0, 2.0, 2.0));
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
		tc = std::make_shared<TransformComponent>(Bridge.get(), Vec3(-30, 10.0f, -10.0f), QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.05, 0.05, 0.05));
		cc = std::make_shared<CollisionComponent>(Bridge.get(), ColliderType::PLANE, 0.0f, Vec3(0.0f, 1.0f, 0.0f));
		Bridge->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Bridge2"));
		Bridge->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("stone"));
		Bridge->AddComponent<ShaderComponent>(shader);
		Bridge->AddComponent(cc);
		Bridge->AddComponent(tc);
		Bridge->OnCreate();
		AddActor(Bridge);

		collisionSystem.SetBridge(Bridge);
		collisionSystem.AddActor(Bridge);
		transformSystem.AddActor(Bridge);
		//Bridge->ListComponents();
	

		/*for (int i = 0; i < 8; i++) {
			float x = -2000.0f - (i * 340.0f);
			Bridge = std::make_shared<Actor>(factory.get());

			tc = std::make_shared<TransformComponent>(nullptr, Vec3(x, 10.0f, -10.0f), QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(4.0, 4.0, 4.0));
			cc = std::make_shared<CollisionComponent>(Bridge.get(), ColliderType::PLANE, 0.0f, Vec3(0.0f, 1.0f, 0.0f));
			Bridge->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Bridge"));
			Bridge->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("stone"));
			Bridge->AddComponent<ShaderComponent>(shader);
			Bridge->AddComponent(cc);
			Bridge->AddComponent(tc);

			Bridge->OnCreate();
			AddActor(Bridge);

			collisionSystem.SetBridge(Bridge);
			collisionSystem.AddActor(Bridge);
			transformSystem.AddActor(Bridge);
			Bridge->ListComponents();
		}
			*/
		

	character = std::make_shared<Actor>(nullptr);
	Quaternion mariosQuaternion = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)) * QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) * QMath::angleAxisRotation(180.0f, Vec3(1.0f, 0.0f, 0.0f));
	pc = std::make_shared<PhysicsComponent>(character.get(), Vec3(-10.0f, 5.0f, -10.0f), mariosQuaternion);
	character->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Square"));
	character->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("RoboGun"));
	character->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("Billboard"));
	character->AddComponent(cc);
	character->AddComponent(pc);
	
	character->OnCreate();
	AddActor(character);
	collisionSystem.SetCharacter(character);
	//character->ListComponents();

	/*projectile = std::make_shared<Actor>(character.get());
	projectile->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Cube"));
	projectile->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BulletSkin"));
	projectile->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));*/


	LoadEnemies();

	//collisionSystem.AddActor(gameboard);
	collisionSystem.AddActor(character);
	collisionSystem.AddActor(Barrel);
	//collisionSystem.AddActor(TestCube);
    collisionSystem.AddActor(factory);
	//collisionSystem.AddActor(Car);

	//transformSystem.AddActor(gameboard);
	physicsSystem.AddActor(Barrel);
	transformSystem.AddActor(factory);
	//transformSystem.AddActor(Car);
	physicsSystem.AddActor(character);


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
	
	/*char c;
	engine = createIrrKlangDevice();
	engine->play2D("./Audio/BackgroundSound.wav");
	engine->setSoundVolume(0.1);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::cout << "hi" << std::endl;*/


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

		/*if (hackingMode) {
			int newX = hackingPlayerPos.x;
			int newY = hackingPlayerPos.y;*/

			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_W: 
				facing = true;
				movingUp = true;
				idleTexture = false;
				//index.y = 1.0f;
				characterPC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.5f, 0.0f));
				//if (hackingPlayerPos.y < hackingTiles.size() - 1) newY++;
				break;
			case SDL_SCANCODE_S:
				facing = true;
				movingDown = true;
				idleTexture = false;
				characterPC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, -0.5f, 0.0f));
				//if (hackingPlayerPos.y > 0) newY--;
				break;
			case SDL_SCANCODE_A:
				facing = true;
				facingLeft = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(-0.25f, 0.0f, 0.0f));
				//if (hackingPlayerPos.x > 0) newX--;
				break;
			case SDL_SCANCODE_D:
				facing = true;
				facingRight = true;
				idleTexture = false;
				//index.x = 0.0f;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.25f, 0.0f, 0.0f));
				//if (hackingPlayerPos.x < hackingTiles[0].size() - 1) newX++;
				break;
			case SDL_SCANCODE_SPACE:
				
				/*if (hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->getNode()->getIsBlocked()) {
					hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->getNode()->setIsBlocked(false);

					
					redTilePositions.erase(
						std::remove(redTilePositions.begin(), redTilePositions.end(),
							std::make_pair(hackingPlayerPos.y, hackingPlayerPos.x)),
						redTilePositions.end()
					);

					
					if (redTilePositions.empty()) {
						
						SDL_Delay(2000);
						hackingMode = false;
						showHackingGrid = false;
						std::cout << "All red tiles cleared. Hacking mode off." << std::endl;
					}*/
				//}

			
				//}

				break;

			}

			// Update player position
		//	if (newX != hackingPlayerPos.x || newY != hackingPlayerPos.y) {
		//		// Reset the old tile
		//		hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->setPathTile(false);

		//		// Move player
		//		hackingPlayerPos.x = newX;
		//		hackingPlayerPos.y = newY;

		//		// Mark new tile as player's position
		//		hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->setPathTile(true);
		//	}
		//}
		//break;
	}

	switch (sdlEvent.type) {
	case SDL_KEYUP:
		//if (!hackingMode) {
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			facing = false;
			movingUp = false;
			idleTexture = true;
			index.y = 0.0f;
			break;
		case SDL_SCANCODE_S:
			facing = false;
			movingDown = false;
			idleTexture = true;
			index.y = 0.0f;
			break;
		case SDL_SCANCODE_A:
			facing = false;
			facingLeft = false;
			idleTexture = true;
			index.x = 0.0f;
			break;
		case SDL_SCANCODE_D:
			facing = false;
			facingRight = false;
			idleTexture = true;
			index.x = 0.0f;
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
	Timing timing("Scene1::Update");
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

	// Get the camera's transform
	Ref<TransformComponent> cameraTransform = camera->GetComponent<TransformComponent>();
	Vec3 cameraPos = cameraTransform->GetPosition();

	// Invert the X-axis movement
	cameraPos.x = -playerPos.x; // Invert player's X position for the camera
	cameraPos.y = -playerPos.y;

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
	/*if (!isGrounded) {
		Vec3 currentVel = playerPhysics->getVel();
		currentVel.y += -9.8f * deltaTime;
		playerPhysics->SetVelocity(currentVel);
	} */


	// Handle horizontal motion (WASD input)
	Vec3 pos = characterTransform->GetPosition();
	Vec3 horizontalMove(0.0f, 0.0f, 0.0f); // Movement direction
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);
	if (!hackingMode) {
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
	}
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
		DecisionTreeNode* tree = pair.second;

		enemy->Update(deltaTime); // This currently does nothing, but still good practice

		if (tree) {
			DecisionTreeNode* result = tree->makeDecision(deltaTime);
			if (auto* action = dynamic_cast<Action*>(result)) {
				action->makeDecision(deltaTime);

				if (action->GetActionName() == "Attack Player") {
					sceneManager->playerHealth -= 0.2f;
					std::cout << "[SCENE1]: Player took damage! Health is now " << sceneManager->playerHealth << "\n";
				}
			}
		}
	}


	if (sceneManager->playerHealth <= 0) {
		sceneManager->dead = true;
	}

	character->Update(deltaTime);
	collisionSystem.Update(deltaTime);
	physicsSystem.Update(deltaTime);
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


