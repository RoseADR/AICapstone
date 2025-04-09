#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene3.h"
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
#include "CollisionActor.h"

#include <chrono>
#include <thread>
#include "CollisionComponent.h"
#include <Quad.h>
#include <QuadMath.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// test test delete me 
using namespace MATH;
using namespace MATHEX;
bool showTutorialPopup = false;

Scene3::Scene3(SceneManager* manager) : sceneManager(manager), engine(nullptr), drawNormals(false), drawOverlay(false),
tileHeight(0.0f), tileWidth(0.0f), xAxis(0.0f), yAxis(0.0f) {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
}


bool Scene3::OnCreate() {
	//Timing timing("Scene1::OnCreate");

	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
	assetManager = std::make_shared<AssetManager>();
	orientationCam = QMath::angleAxisRotation(10.0f, Vec3(1.0f, 0.0f, 0.0f));
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -10.0f), orientationCam);
	camera->OnCreate();
	camera->GetProjectionMatrix().print("ProjectionMatrix");
	camera->GetViewMatrix().print("ViewMatrix");

	Vec3 dir = Vec3(-1.0f, 0.0f, -1.0f); // Soft diagonal light
	Vec4 color = Vec4(1.0f, 1.0f, 1.0f, 1.0f); // Softer white light

	light = std::make_shared<LightActor>(camera.get(), LightStyle::DirectionLight, dir, color);
	light->OnCreate();

	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	Ref<CollisionComponent> cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::Sphere, 1.0f);
	Ref<PhysicsComponent> pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(0.0f, 0.0f, 4.1f), orientationBoard);
	Ref<TransformComponent> tc = std::make_shared<TransformComponent>(nullptr, Vec3(-10.0f, -10.0f, 0.0f), orientationBoard);

	////////////////////////////////////////////////////////////////BEGINING OF ACTORS IN SCENE/////////////////////////////////////////////////////////////////////////////
	tutorialTrigger = std::make_shared<Actor>(nullptr);

	// Set it where the player should touch it
	Vec3 triggerPos = Vec3(-19.0f, 0.0f, -8.0f); // Adjust this position
	Vec3 triggerScale = Vec3(0.0f, 0.0f, 0.0f); // Invisible (scale 0)

	tutorialTrigger->AddComponent<PhysicsComponent>(nullptr, triggerPos, QMath::angleAxisRotation(0.0f, Vec3(0, 1, 0)), Vec3(), Vec3(), Vec3(), triggerScale);
	tutorialTrigger->AddComponent<CollisionComponent>(tutorialTrigger.get(), ColliderType::Sphere, 8.0f); // Radius of detection
	tutorialTrigger->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	tutorialTrigger->AddComponent<ShaderComponent>(shader);
	tutorialTrigger->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	tutorialTrigger->SetName("TutorialTrigger");
	tutorialTrigger->OnCreate();

	AddActor(tutorialTrigger);
	collisionSystem.AddActor(tutorialTrigger);
	physicsSystem.AddActor(tutorialTrigger);
	transformSystem.AddActor(tutorialTrigger);


	Ref<Actor> bus = std::make_shared<Actor>(nullptr);

	Vec3 busPos = Vec3(40.0f, 2.4f, -7.0f);
	Quaternion busRot = QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f));
	Vec3 busScale = Vec3(4.0f, 3.0f, 4.0f); 

	tc = std::make_shared<TransformComponent>(bus.get(), Vec3(40.0f, 2.4f, -7.0f), QMath::angleAxisRotation(0.0f, Vec3(1, 0, 0)) * QMath::angleAxisRotation(45.0f, Vec3(0, 0, 1)), Vec3(0.2f, 0.2f, 0.2f));
	cc = std::make_shared<CollisionComponent>(bus.get(), ColliderType::AABB);

	cc->SetAABB(busPos, busRot, busScale.x / 2, busScale.y / 2, busScale.z / 2);
	bus->AddComponent(cc);
	bus->AddComponent(tc);
	bus->AddComponent(assetManager->GetComponent<MeshComponent>("Bus"));             
	bus->AddComponent<ShaderComponent>(shader);
	bus->AddComponent(assetManager->GetComponent<MaterialComponent>("BusDif")); 
	bus->SetName("Bus");
	bus->OnCreate();
	AddActor(bus);
	collisionSystem.AddActor(bus);
	transformSystem.AddActor(bus);


	// === Car Actor (AABB) ===
	Ref<Actor> car = std::make_shared<Actor>(nullptr);

	Vec3 carPos = Vec3(21.7f, -2.0f, -7.0f);
	Quaternion carRot = QMath::angleAxisRotation(0.0f, Vec3(1, 0, 0));
	Vec3 carScale = Vec3(7.0f, 7.0f, 7.0f);
	Vec3 collisionScale = Vec3(4.0f, 1.0f, 2.0f);

	tc = std::make_shared<TransformComponent>(car.get(), carPos, QMath::angleAxisRotation(180.0f, Vec3(1, 0, 0)) * QMath::angleAxisRotation(-26.0f, Vec3(0, 0, 1)), carScale);
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



	for (int i = 0; i < 3; i++) {
		float x = 0.0f - (i * 90.0f);
		bg = std::make_shared<Actor>(nullptr);
		orientationBg = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
		bg->AddComponent<TransformComponent>(nullptr, Vec3(x, 30.0f, -70.0f), orientationBoard, Vec3(15.0f, 15.0f, 1.0f));
		bg->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
		bg->AddComponent<ShaderComponent>(shader);
		bg->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
		AddActor(bg);
	}


	// === House Actor (AABB) ===
	Ref<Actor> house = std::make_shared<Actor>(nullptr);

	Vec3 housePos = Vec3(-30.0f, 0.0f, -10.0f);
	Quaternion houseRot = QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f));
	Vec3 houseScale = Vec3(1.05f, 1.05f, 1.05f);
	Vec3 collisonScale = Vec3(5.0f, 5.0f, 5.0f);

	tc = std::make_shared<TransformComponent>(house.get(), housePos, houseRot, houseScale);
	cc = std::make_shared<CollisionComponent>(house.get(), ColliderType::AABB);
	cc->SetAABB(housePos, houseRot, collisionScale.x / 2, collisionScale.y / 2, collisionScale.z / 2);
	house->AddComponent(cc);
	house->AddComponent(tc);
	house->AddComponent(assetManager->GetComponent<MeshComponent>("House"));
	house->AddComponent<ShaderComponent>(shader);
	house->AddComponent(assetManager->GetComponent<MaterialComponent>("houseText"));
	house->SetName("House");
	house->OnCreate();
	AddActor(house);
	collisionSystem.AddActor(house);
	transformSystem.AddActor(house);

	
	Ref<Actor> grass = std::make_shared<Actor>(nullptr);

	Vec3 grassPos = Vec3(-20.0f, -3.5f, -7.0f);
	Quaternion grassRot = QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f));
	Vec3 grassScale = Vec3(16.0f, 1.0f, 16.0f);

	tc = std::make_shared<TransformComponent>(grass.get(), grassPos, grassRot, grassScale);
	cc = std::make_shared<CollisionComponent>(grass.get(), ColliderType::PLANE, 0.0f, Vec3(0.0f, 1.0f, 0.0f), -3.5f);
	grass->AddComponent(cc);
	grass->AddComponent(tc);
	grass->AddComponent(assetManager->GetComponent<MeshComponent>("Plane"));
	grass->AddComponent<ShaderComponent>(shader);
	grass->AddComponent(assetManager->GetComponent<MaterialComponent>("grass"));
	grass->SetName("Grass");
	grass->OnCreate();
	AddActor(grass);
	collisionSystem.AddActor(grass);
	transformSystem.AddActor(grass);

	// === Fences Actor ===
	Ref<Actor> fences = std::make_shared<Actor>(nullptr);

	Vec3 fencePos = Vec3(0.0f, -3.25f, -7.0f); // Adjust position
	Quaternion fenceRot = QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)); // Rotates to lay flat
	Vec3 fenceScale = Vec3(0.07f, 0.07f, 0.07f); // Adjust size as needed

	Ref<TransformComponent> fenceTC = std::make_shared<TransformComponent>(fences.get(), fencePos, fenceRot, fenceScale);
	fences->AddComponent(fenceTC);
	fences->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Fences")); // Use "Fence" if you have a specific one
	fences->AddComponent<ShaderComponent>(shader);
	fences->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WoodBox")); // Make sure this material exists

	fences->SetName("Fences");
	fences->OnCreate();
	AddActor(fences);


	int rows = 5;
	int cols = 14;
	float spacingX = 5.0f;
	float spacingZ = 5.0f;
	Vec3 forestStart = Vec3(-25.0f, -3.25f, -35.0f);

	int treeID = 0;

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			Ref<Actor> tree = std::make_shared<Actor>(nullptr);

			float stagger = (i % 2 == 0) ? 0.0f : spacingX / 2.0f;
			Vec3 pos = forestStart + Vec3(j * spacingX + stagger, 0.0f, i * spacingZ);

			Quaternion rot = QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f));
			Vec3 scale = Vec3(0.01f, 0.01f, 0.01f);

			const char* meshName;
			const char* materialName;

			int treeChance = rand() % 10;
			if (treeChance < 7) { 
				meshName = (rand() % 2 == 0) ? "Tree" : "Tree3";
				materialName = "tallTree";
			}
			else { 
				meshName = (rand() % 2 == 0) ? "Tree2" : "Tree4";
				materialName = "roundTree";
			}

			tree->AddComponent(std::make_shared<TransformComponent>(tree.get(), pos, rot, scale));
			tree->AddComponent(assetManager->GetComponent<MeshComponent>(meshName));
			tree->AddComponent<ShaderComponent>(shader);
			tree->AddComponent(assetManager->GetComponent<MaterialComponent>(materialName));
			tree->SetName("Tree" + std::to_string(treeID++));
			tree->OnCreate();
			AddActor(tree);
		}
	}

		sceneChangeTrigger = std::make_shared<Actor>(nullptr);
		Vec3 sctriggerPos = Vec3(47.0f, 10.0f, -7.0f); // Adjust as needed
		Vec3 sctriggerScale = Vec3(0.0f, 0.0f, 0.0f);

		sceneChangeTrigger->AddComponent<PhysicsComponent>(nullptr, sctriggerPos, QMath::angleAxisRotation(0.0f, Vec3(0, 1, 0)), Vec3(), Vec3(), Vec3(), sctriggerScale);
		sceneChangeTrigger->AddComponent<CollisionComponent>(sceneChangeTrigger.get(), ColliderType::Sphere, 1.0f); // Detection radius
		sceneChangeTrigger->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Box"));
		sceneChangeTrigger->AddComponent<ShaderComponent>(shader);
		sceneChangeTrigger->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("grass")); // or any mat
		sceneChangeTrigger->SetName("SceneChangeTrigger");
		sceneChangeTrigger->OnCreate();

		AddActor(sceneChangeTrigger);
		collisionSystem.AddActor(sceneChangeTrigger);
		physicsSystem.AddActor(sceneChangeTrigger);
		transformSystem.AddActor(sceneChangeTrigger);

		//// === Bridge Actor (AABB) ===
		Bridge = std::make_shared<Actor>(nullptr);

		Vec3 boxPosB = Vec3(-129.0f, 8.0f, -8.0f); // location
		Vec3 boxScaleB = Vec3(1.0f, 1.0f, 1.0f); // cube size
		Quaternion boxRotB = QMath::angleAxisRotation(0.0f, Vec3(1, 0, 0));
		Vec3 cs = Vec3(1.0f, 1.0f, 1.0f);

		tc = std::make_shared<TransformComponent>(Bridge.get(), Vec3(52.0, -2.66f, -8.0f),
			QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(0.30, 0.30, 0.30));
		cc = std::make_shared<CollisionComponent>(Bridge.get(), ColliderType::AABB);
		cc->SetAABB(boxPosB, boxRotB, boxScaleB.x / 2, boxScaleB.y / 2, boxScaleB.z / 2); // set AABB
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



		for (int i = 0; i < 8; i++) {
			float x = 180.0f - (i * 37.0f);
			auto road = std::make_shared<Actor>(Bridge.get());

			road->AddComponent<TransformComponent>(nullptr, Vec3(x, 34.0f, 2.4f), QMath::angleAxisRotation(270.0f, Vec3(1.0f, 0.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)), Vec3(3.72, 20.0, 3.8));
			road->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
			road->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));
			road->AddComponent<ShaderComponent>(shader);
			road->OnCreate();
			AddActor(road);
		}

		character = std::make_shared<Actor>(nullptr);
		Quaternion mariosQuaternion = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)) * QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f))
			* QMath::angleAxisRotation(180.0f, Vec3(1.0f, 0.0f, 0.0f));
		pc = std::make_shared<PhysicsComponent>(character.get(), Vec3(0.0f, 0.0f, -7.0f),
			mariosQuaternion, Vec3(), Vec3(), Vec3(), Vec3(3.0f, 3.0f, 3.0f));
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
		physicsSystem.AddActor(character);
		collisionSystem.AddActor(character);

		SpawnAmmoAt(Vec3(15.0f, -3.1f, -8.0f));
		SpawnAmmoAt(Vec3(10.0f, -3.1f, -8.0f));
		//SpawnAmmoAt(Vec3(-160.0f, 12.2f, -6.0f));
	

		
		engine = createIrrKlangDevice();
		//engine->play2D("./Audio/SciFiBG.mp3");
		engine->setSoundVolume(0.1);
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		//std::cout << "hi" << std::endl;

		return true;
}

Scene3::~Scene3() {
	Debug::Info("Deleted Scene1: ", __FILE__, __LINE__);
	OnDestroy();
}

void Scene3::OnDestroy() {
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

void Scene3::FireProjectile() {
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
	projectile->AddComponent<TransformComponent>(nullptr, charPos, Quaternion(), Vec3(0.5, 0.5, 0.5));
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


void Scene3::Reload()
{
	int bulletsNeeded = sceneManager->clipSize - sceneManager->clipAmmo;
	if (sceneManager->totalAmmo > 0 && bulletsNeeded > 0) {
		int bulletsToReload = std::min(bulletsNeeded, sceneManager->totalAmmo);
		sceneManager->clipAmmo += bulletsToReload;
		sceneManager->totalAmmo -= bulletsToReload;
	}

}



void Scene3::HandleEvents(const SDL_Event& sdlEvent) {
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

		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
	
				movingDown = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.5f, 0.0f));
			
			break;

		case SDL_SCANCODE_S:
			
				movingDown = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, -0.5f, 0.0f));
		

			break;

		case SDL_SCANCODE_A:

		
				facing = false;
				facingLeft = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(-0.25f, 0.0f, 0.0f));
		
			break;

		case SDL_SCANCODE_D:

			
				facing = true;
				facingRight = true;
				idleTexture = false;
				characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.25f, 0.0f, 0.0f));
		

			break;

		case SDL_SCANCODE_SPACE:
			std::cout << "SPACEBAR PRESSED!\n";
		
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


			break;
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
			facingLeft = false;
			idleTexture = true;

			// Only play footstep sound if it's not already playing
			/*if (!footstepSound || footstepSound->isFinished()) {
				footstepSound = engine->play2D("./Audio/grassFootsteps.wav", false, false, true);
			}*/
			break;

		case SDL_SCANCODE_D:
			facingRight = false;
			idleTexture = true;

			/*if (!footstepSound || footstepSound->isFinished()) {
				footstepSound = engine->play2D("./Audio/grassFootsteps.wav", false, false, true);
			}*/
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

void Scene3::Update(const float deltaTime) {
	//Timing timing("Scene1::Update");
	frameCount++;

	// Player-hack collision detection
	if (footstepTimer > 0.0f) {
		footstepTimer -= deltaTime;
	}

	


	auto playerPhysics = character->GetComponent<PhysicsComponent>();
	auto playerCollision = character->GetComponent<CollisionComponent>();

	auto triggerPhysics = tutorialTrigger->GetComponent<PhysicsComponent>();
	auto triggerCollision = tutorialTrigger->GetComponent<CollisionComponent>();

	if (playerPhysics && playerCollision && triggerPhysics && triggerCollision) {
		Sphere playerSphere{ playerPhysics->GetPosition(), playerCollision->GetRadius() };
		Sphere triggerSphere{ triggerPhysics->GetPosition(), triggerCollision->GetRadius() };

		// Show or hide popup based on whether player is inside trigger
		if (collisionSystem.SphereSphereCollisionDetection(playerSphere, triggerSphere)) {
			showTutorialPopup = true;
		}
		else {
			showTutorialPopup = false;
		}
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

	}
		


	Vec3 playerPos = playerPhysics->GetPosition();



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

			ammo->OnDestroy();

			// Remove from actors and ammo list
			actors.erase(std::remove_if(actors.begin(), actors.end(),
				[&](const Ref<Actor>& a) { return a == ammo; }), actors.end());

			ammoPickups.erase(ammoPickups.begin() + i);
			engine->play2D("./Audio/ammoPickUp.wav");
		}
		else {
			++i;
		}
	}
	auto sctriggerPhysics = sceneChangeTrigger->GetComponent<PhysicsComponent>();
	auto sctriggerCollision = sceneChangeTrigger->GetComponent<CollisionComponent>();

	if (!loadingStarted && playerPhysics && playerCollision && sctriggerPhysics && sctriggerCollision) {
		Sphere playerSphere{ playerPhysics->GetPosition(), playerCollision->GetRadius() };
		Sphere triggerSphere{ sctriggerPhysics->GetPosition(), sctriggerCollision->GetRadius() };

		if (collisionSystem.SphereSphereCollisionDetection(playerSphere, triggerSphere)) {
			SpawnLoadingScreen();
			loadingStarted = true;
			loadingTimer = 0.0f;

			// Make the camera match the loading screen's rotation
			if (camera && loadingScreenActor) {
				auto camTransform = camera->GetComponent<TransformComponent>();
				if (camTransform) {
					// Manually input the same rotation used in SpawnLoadingScreen()
					Quaternion screenRot = QMath::angleAxisRotation(270.0f, Vec3(0.0f, 1.0f, 0.0f)) *
						QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 0.0f, 1.0f));

					Vec3 currentCamPos = camTransform->GetPosition();
					camTransform->SetTransform(currentCamPos, screenRot);
					camera->UpdateViewMatrix();
				}
			}
		}
	}

	if (loadingStarted) {
		loadingTimer += deltaTime;

		if (loadingTimer >= 2.0f) {
			sceneManager->triggerScene1 = true; //  only set the flag here
		}
		return; //  prevent the rest of Update() from running
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
			vel.y = -9.8f;
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
				engine->play2D("./Audio/grassFootsteps.wav", false);
				footstepTimer = footstepCooldown;
			}
		}
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

	character->Update(deltaTime);
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

	// Run all collisions
	collisionSystem.Update(deltaTime);
	physicsSystem.Update(deltaTime);
}



void Scene3::Render() const {
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

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (showTutorialPopup) {
		ImVec2 displaySize = ImGui::GetIO().DisplaySize;
		ImVec2 windowPos = ImVec2(displaySize.x * 0.5f, displaySize.y - 20.0f);

		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));
	

		ImGui::Begin("##TutorialTextOnly", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoDecoration);

	
		ImGui::SetWindowFontScale(2.5f);

		ImGui::Text("Welcome to the tutorial!");
		ImGui::Text("Use WASD to move.");
		ImGui::Text("Press Space to jump.");
		ImGui::Text("Left Click to shoot.");

		ImGui::End();
	}


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



	// ** Render Collision Boxes (Wireframe Mode) **
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (auto actor : actors) {
		auto collider = actor->GetComponent<CollisionComponent>();
		if (collider && collider->GetColliderType() == ColliderType::AABB) {
			collider->DrawAABB(); 
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Reset polygon mode


	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);  // Ensure fixed-function pipeline for tiles



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

void Scene3::SpawnLoadingScreen() {
	loadingScreenActor = std::make_shared<Actor>(nullptr);

	// Set position and rotation
	Vec3 screenPos = Vec3(20.02f, 13.28f, 9.1f);

	// Rotate plane to face along +X (normal = +X)
	Quaternion screenRotation = QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)) * QMath::angleAxisRotation(0.0f, Vec3(0.0f, 0.0f, 1.0f));

	loadingScreenActor->AddComponent<TransformComponent>(nullptr, screenPos, screenRotation, Vec3(4.5f, 4.53f, 4.53f));
	loadingScreenActor->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	loadingScreenActor->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	loadingScreenActor->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("Loading"));
	loadingScreenActor->SetName("LoadingScreen");
	loadingScreenActor->OnCreate();
	AddActor(loadingScreenActor);
}




void Scene3::DrawMeshOverlay(const Vec4 color) const {
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


void Scene3::DrawNormals(const Vec4 color) const {
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

void Scene3::SpawnAmmoAt(const Vec3& position) {
	auto ammo = std::make_shared<Actor>(nullptr);

	Quaternion rotation = QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f));
	Vec3 velocity = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);

	ammo->AddComponent<PhysicsComponent>(nullptr, position, rotation, velocity, Vec3(), Vec3(), scale);
	ammo->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Ammo"));
	ammo->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("AmmoDif"));
	ammo->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	ammo->AddComponent<CollisionComponent>(ammo.get(), ColliderType::Sphere, 1.0f);

	ammo->OnCreate();
	AddActor(ammo);
	collisionSystem.AddActor(ammo);
	physicsSystem.AddActor(ammo);
	ammoPickups.push_back(ammo);

}




int Scene3::Pick(int x, int y) {
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
	glReadPixels(x, viewport.height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &colorIndex);
	colorIndex &= 0x00FFFFFF; /// This zeros out the alpha component

	if (colorIndex == 0x00FFFFFF) return -1; /// Picked nothing
	else return colorIndex;
}


