#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene1.h"
#include "MMath.h"
#include "Debug.h"
#include "CameraActor.h"
#include "LightActor.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "MaterialComponent.h"
#include "MMath.h"
#include "AiComponent.h"
#include "LocationManager.h"
#include "Timing.h"

#include "ScottCollisionComponent.h"
#include "ScottCollisionSystem.h"

#include "CollisionSystem.h"

#include <chrono>
#include <thread>
#include "CollisionComponent.h"

// test test delete me 
using namespace MATH;


Scene1::Scene1():engine(nullptr), drawNormals(false), drawOverlay(false){
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
}

bool Scene1::OnCreate() {
	
	//Timing timing("Scene1::OnCreate");

	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
	assetManager = std::make_shared<AssetManager>();
	orientationCam = QMath::angleAxisRotation(15.0f, Vec3(1.0f, 0.0f, 0.0f));
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -7.0f, -13.0f), orientationCam);
	camera->OnCreate();
	camera->GetProjectionMatrix().print("ProjectionMatrix");
	camera->GetViewMatrix().print("ViewMatrix");

	light = std::make_shared<LightActor>(nullptr, LightStyle::DirectionLight, Vec3(3.0f, 5.0f, -5.0f),Vec4(0.9f,0.9f,0.9f,0.0f));
	light->OnCreate();


	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	Ref<CollisionComponent> cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::Sphere, 0.5f);
	Ref<PhysicsComponent> pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(0.0f, 0.0f, 4.1f), orientationBoard);

	//bg = std::make_shared<Actor>(nullptr);
	orientationBg = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
	//// Add the transform and other components
	//bg->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 50.0f, -70.0f), orientationBoard, Vec3(14.5f, 14.5f, 10.0f));
	//bg->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	//bg->AddComponent<ShaderComponent>(shader);
	//bg->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	//AddActor(bg);

	gameboard = std::make_shared<Actor>(nullptr);
	
	orientationBoard = QMath::angleAxisRotation(276.0f, Vec3(1.0f, 0.0f, 0.0f));
	gameboard->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.1f, -10.0f), orientationBoard, Vec3(1.0, 1.0, 1.0));
	gameboard->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	gameboard->AddComponent<ShaderComponent>(shader);
	gameboard->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));
	gameboard->AddComponent(cc);
	// <CollisionComponent>(nullptr,
	//	ColliderType::Sphere, Vec3(9.0f, 2.0f, 22.5f), // Width, height, depth
	//	0.0f, Vec3(-5.0f, 0.0f, 0.0f)); // Offset
	// 
	AddActor(gameboard);


	factory = std::make_shared<Actor>(gameboard.get());

	factory->AddComponent<TransformComponent>(nullptr, Vec3(30.0f, 0.1f, 0.0f), orientationBg, Vec3(0.05, 0.05, 0.05));
	factory->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Factory"));
	factory->AddComponent<ShaderComponent>(shader);
	factory->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("bg"));
	factory->AddComponent(cc);
	// <CollisionComponent>(factory.get(),
	////	ColliderShape::AABB, Vec3(9.0f, 2.0f, 22.5f), // Width, height, depth
	////	0.0f, Vec3(-5.0f, 0.0f, 0.0f)); // Offset

	AddActor(factory);
	
	//house = std::make_shared<Actor>(nullptr);
	orientationHouse = QMath::angleAxisRotation(10.0f, Vec3(1.0f, 0.0f, 0.0f));

	// Add the transform and other components
	//house->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 1.1f, -12.0f), orientationHouse, Vec3(1.2f, 1.2f, 1.2f));
	//house->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("House"));
	//house->AddComponent<ShaderComponent>(shader);
	//house->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("houseText"));
	//house->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("houseTextNor"));
	////house->AddComponent<CollisionComponent>(house.get(),
	////	ColliderShape::AABB, Vec3(9.0f, 9.0f, 22.5f), // Width, height, depth
	////	0.0f, Vec3(0.0f, 0.0f, 0.0f)); // Offset 
	//AddActor(house);

	//bill = std::make_shared<Actor>(nullptr);
	orientationBill = QMath::angleAxisRotation(1800.0f, Vec3(0.0f, 1.0f, 0.0f));
	/*bill->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 5.1f, -17.0f), orientationBill, Vec3(0.3, 0.3, 0.3));
	bill->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Board"));
	bill->AddComponent<ShaderComponent>(shader);
	bill->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("billAds"));
	AddActor(bill);*/
	
	Vec3 boardPos =  actors[0]->GetComponent<TransformComponent>()->GetPosition();
	//std::cout << "GameBoard Position: ("
		/*<< boardPos.x << ", "
		<< boardPos.y << ", "
		<< boardPos.z << ")\n"*/;

	TestCube = std::make_shared<Actor>(factory.get());
	
	TestCube->AddComponent<TransformComponent>(nullptr, Vec3(-360.0f, 30.0f, 15.0f), orientationBill, Vec3(20.0, 20.0, 20.0));
	TestCube->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Box"));
	TestCube->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WoodBox"));
	TestCube->AddComponent<ShaderComponent>(shader);
	TestCube->AddComponent(cc);
		/*<CollisionComponent>(TestCube.get(), ColliderType::Sphere,
		Vec3(1.0f, 1.0f, 1.0f), 0.0f, Vec3(0.0f, 0.0f, 0.0f));*/

	AddActor(TestCube);


	character = std::make_shared<Actor>(gameboard.get());
	Quaternion mariosQuaternion = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) * QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
	pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(0.0f, 0.0f, 4.1f), mariosQuaternion);
	character->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Mario"));
	character->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("WalkSpriteSheet"));
	character->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("Billboard"));
	character->AddComponent(cc);
	character->AddComponent(pc);
	// <CollisionComponent>(nullptr, 
	//	ColliderType::Sphere, /*Vec3(1.0f, 1.0f, 2.0f), */
	//	0.0f/*, Vec3(0.0f, 0.5f, 0.0f))*/);
	AddActor(character);

	LoadEnemies();

	collisionSystem.AddActor(gameboard);
	collisionSystem.AddActor(character);
	collisionSystem.AddActor(TestCube);
	collisionSystem.AddActor(factory);

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

	Actor* enemy = actors[2].get();    // Assuming actors[2] is the enemy
	Actor* player = character.get(); // Assuming character is the player

	std::cout << "[LOG]: Building Decision Tree..." << std::endl;
	decisionTreeRoot = TreeBuilder::buildTree("Scene1.xml", enemy, player);
	if (!decisionTreeRoot) {
		std::cerr << "[ERROR]: Failed to build decision tree" << std::endl;
		//return;
	}
	std::cout << "[LOG]: Decision tree successfully built" << std::endl;

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
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
	OnDestroy();
}

void Scene1::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);

	//FOR PATHFINDING
	// Delete each tile and its associated node in the 2D tiles vector
	for (auto& row : tiles) {
		for (Tile* tile : row) {
			delete tile->getNode();  // Delete the node associated with each tile
			delete tile;              // Delete the tile itself
		}
	}
	//Delete the graph
	delete graph;

	//engine->drop(); // delete engine
	return;
}

void Scene1::HandleEvents(const SDL_Event &sdlEvent) {
	//Timing timing("Scene1::HandleEvents");

	static int objID = -1;
	static Vec2 currentMousePos;
	static Vec2	lastMousePos;
	static float flip = 1.0f;
	Ref<TransformComponent> cameraTC;
	Ref<PhysicsComponent> characterTC;
	Ref<TransformComponent> gameBoardTC;

	Vec3 rotatedDirection;
	Vec3 direction;  
	Quaternion start;
	Quaternion end;

	Quaternion orientationU;
	Quaternion orientationL;
	Quaternion orientationR;
	Quaternion orientationD;


	// was going to use to store the previous position to calulate vel - didnt work
	//Vec3 currentPosition = characterTC->GetPosition();

	/// Handle Camera movement 
	switch (sdlEvent.type) {


	case SDL_KEYDOWN:
		cameraTC = camera->GetComponent<TransformComponent>();
		characterTC = character->GetComponent<PhysicsComponent>();
		gameBoardTC = gameboard->GetComponent<TransformComponent>();
		

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
		//	
		//case SDL_SCANCODE_W:
		//	orientationU = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)); // Facing upward
		//	characterTC->SetTransform(characterTC->GetPosition(), orientationU);
		//	break;
	

		//case SDL_SCANCODE_A:
		//	orientationL = QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)) *   // Turn left
		//		QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
		//	characterTC->SetTransform(characterTC->GetPosition(), characterTC->GetQuaternion());
		//
		//	break;


		//case SDL_SCANCODE_S:

		//	//characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, -0.1f, 0.0f));
		//	orientationD = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn to face backward
		//		QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));     
		//	characterTC->SetTransform(characterTC->GetPosition(), characterTC->GetQuaternion());
		//	break;


		//case SDL_SCANCODE_D:

		//	//characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f));
		//	orientationR = QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn right
		//		QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
		//	characterTC->SetTransform(characterTC->GetPosition(), characterTC->GetQuaternion());
		//	
		//	break;

		case SDL_SCANCODE_R:

			characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.0f, -0.1f));
			orientationR = QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn right
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
			characterTC->SetTransform(characterTC->GetPosition(), characterTC->GetQuaternion());
			
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

		case SDL_JOYBUTTONDOWN :
			if (sdlEvent.jbutton.button == SDL_CONTROLLER_BUTTON_A)
			{
			
			}
			break;

		case SDL_JOYAXISMOTION:
			if (sdlEvent.jaxis.which == 0)
			{
			
				xAxis = sdlEvent.jaxis.value;
				std::cout << xAxis << '\n';
				/*characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f));
				characterTC->SetTransform(characterTC->GetPosition() + Vec3(xAxis, 0.0f, 0.0f), orientationL);*/
				
				
			}
			else if (sdlEvent.jaxis.which == 1) {
				yAxis = sdlEvent.jaxis.value;
				std::cout << yAxis << '\n';
				/*characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, -0.1f, 0.0f));
				characterTC->SetTransform(characterTC->GetPosition() + Vec3(0.0f, yAxis, 0.0f), orientationU);*/
			}

			break;


    }

}

void Scene1::Update(const float deltaTime) {
	//Timing timing("Scene1::Update");
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


	//
	//for (size_t i = 0; i < actors.size(); ++i) {
 //       for (size_t j = i + 1; j < actors.size(); ++j) {
 //           if (CollisionSystem::SphereSphereCollisionDetection(actors[i], actors[j])) {
 //               CollisionSystem::SphereSphereCollisionResponse(actors[i], actors[j]);
 //           }
 //       }
 //   }
	//if (CollisionSystem::SphereSphereCollisionDetection(character, TestCube)) {
	//	std::cout << "TestCube Collision Detected!" << std::endl;
	//	CollisionSystem::SphereSphereCollisionResponse(character, TestCube);
	//}

	for (auto actor : actors) {
		auto transform = actor->GetComponent<TransformComponent>();
		if (transform) {
			Vec3 pos = transform->GetPosition();
			std::cout << "[Scene1] Actor Position Updated: ("
				<< pos.x << ", " << pos.y << ", " << pos.z << ")\n";
		}
	}


	Ref<PhysicsComponent> playerTransform = character->GetComponent<PhysicsComponent>();
	Vec3 playerPos = playerTransform->GetPosition(); 

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

	const float gravity = -9.8f;        // Gravitational acceleration
	const float jumpStrength = 5.0f;   // Initial jump velocity
	const float moveSpeed = 3.0f;      // Movement speed
	static float verticalVelocity = 0.0f; // Character's vertical velocity
	

	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);


	auto characterTransform = character->GetComponent<TransformComponent>();


	/*if (characterTransform) {
		bool p = true;
		p = CollisionHandler::CheckCollision(character, TestCube);

		if (p) {
			CollisionHandler::ResolveCollision(character, TestCube);
		}
	}*/

	if (characterTransform) {
		Vec3 pos = characterTransform->GetPosition();

		// Check if the character is on the floor
		/*isGrounded = CollisionSystem::SphereSphereCollisionDetection(character, gameboard) || CollisionSystem::SphereSphereCollisionDetection(character, TestCube);*/

		// Handle vertical motion (gravity and jump)
		if (isGrounded) {
			// Reset vertical velocity when grounded
			verticalVelocity = 0.0f;

			// Jump if space is pressed
			const Uint8* keystate = SDL_GetKeyboardState(nullptr);
			if (keystate[SDL_SCANCODE_SPACE]) {
				verticalVelocity = jumpStrength; // Apply jump velocity
				isGrounded = false;              // Character is no longer grounded
			}

			// Resolve collision to align with the floor
			//CollisionSystem::SphereSphereCollisionResponse(character, gameboard);
		}
		else {
			// Apply gravity when not grounded
			//verticalVelocity += gravity * deltaTime;
		}

		// Handle horizontal motion (WASD input)
		Vec3 horizontalMove(0.0f, 0.0f, 0.0f); // Movement direction
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);
		if (keystate[SDL_SCANCODE_W]) horizontalMove.y += 1.0f; // Forward
		if (keystate[SDL_SCANCODE_S]) horizontalMove.y -= 1.0f; // Backward
		if (keystate[SDL_SCANCODE_A]) horizontalMove.x -= 1.0f; // Left
		if (keystate[SDL_SCANCODE_D]) horizontalMove.x += 1.0f; // Right

		// Normalize movement direction and scale by speed and deltaTime
		if (VMath::mag(horizontalMove) > 0.0f) {
			horizontalMove = VMath::normalize(horizontalMove) * moveSpeed * deltaTime;
		}

		// Combine horizontal and vertical motion
		pos += horizontalMove;        // Update horizontal position
		pos.z += verticalVelocity * deltaTime; // Update vertical position

		// Apply updated position to the character
		characterTransform->SetPosition(pos);
	}

	Ref<TransformComponent>characterTC = character->GetComponent<TransformComponent>();
	Ref<PhysicsComponent> enemyTC;

	collisionSystem.Update(deltaTime);
	// Update the gameboard transform
	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);

	// Get Mario's position (character's position)
	Vec3 mariosPos = character->GetComponent<PhysicsComponent>()->GetPosition();
	locationManager.mariosPos = mariosPos;

	// Enemy 1 (actors[2]) Decision Tree Logic 
	auto& enemy = actors[2]; 

	// Update Enemy 1's state
	enemy->Update(deltaTime); // THIS DOES NOTHING 

	// Evaluate the decision tree
	if (decisionTreeRoot) {
		DecisionTreeNode* result = decisionTreeRoot->makeDecision(deltaTime);
		if (auto* action = dynamic_cast<Action*>(result)) {
			action->makeDecision(deltaTime); // Execute the encapsulated action logic
		}
		else {
			std::cerr << "[ERROR]: Decision tree evaluation returned a non-action node.\n";
		}
	}
	
	currentTime += deltaTime;
	index = static_cast <int> (currentTime / frameSpeed) % 8;
}


void Scene1::Render() const {
	//Timing timing("Scene1::Render");

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS); // JUST ADDED FOR TESTING ROTATION
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatriciesID());
	glBindBuffer(GL_UNIFORM_BUFFER, light->GetLightID());

	for (auto actor : actors) {
		glUseProgram(actor->GetComponent<ShaderComponent>()->GetProgram());
		glUniformMatrix4fv(actor->GetComponent<ShaderComponent>()->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
		glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
		glUniform1f(actor->GetComponent<ShaderComponent>()->GetUniformID("index"), index);
		actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);

	}

	// ** Render Collision Boxes **
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Set wireframe mode
	glDisable(GL_TEXTURE_2D); // Disable textures to draw wireframes clearly
	glColor3f(1.0f, 0.0f, 0.0f); // Red color for collision boxes

	for (auto actor : actors) {
		auto collider = actor->GetComponent<CollisionComponent>();
		if (collider) {
			collider->Render(); // Draw the bounding box
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset polygon mode to fill
	glEnable(GL_TEXTURE_2D); // Re-enable textures
	glUseProgram(0); // Unbind any shaders



	//glBindTexture(GL_TEXTURE_2D, 0);
	//glUseProgram(0);

	//for (auto actor : actors) {
	//	auto collider = actor->GetComponent<CollisionComponent>();
	//	if (collider) {
	//		collider->Render(); // Render collision boxes
	//	}
	//}
	////glPushMatrix();

	if (showTiles) { // Only render tiles if showTiles is true
		for (const auto& row : tiles) {
			for (Tile* tile : row) {
				tile->render(); // Render the tile
			}
		}
	}

	//glPopMatrix(); // Restore the previous matrix state

	if (drawOverlay == true) {
		DrawMeshOverlay(Vec4(1.0f, 1.0f, 1.0f, 0.5f));
	}

	if (drawNormals == true) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.01f));
	}
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
	Ref<MeshComponent> e = assetManager->GetComponent<MeshComponent>("Sphere");
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	Ref<MaterialComponent> enemyTexture = assetManager->GetComponent<MaterialComponent>("BlackChessTexture");

	// Create an array of 2 enemy actors
	Ref<Actor> enemies[2]{};

	// Set up the first enemy
	enemies[0] = std::make_shared<Actor>(gameboard.get()); // Make actor and parent it to gameboard
	enemies[0]->AddComponent<ShaderComponent>(shader); // Add shader
	enemies[0]->AddComponent<MaterialComponent>(enemyTexture); // Add texture
	enemies[0]->AddComponent<AiComponent>(enemies[0].get()); // Add AI component
	enemies[0]->AddComponent<MeshComponent>(e); // Add mesh
	enemies[0]->AddComponent<PhysicsComponent>(nullptr, Vec3(-5.0f, 0.0f, 0.05f),
		QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));

	// Set up the second enemy
	enemies[1] = std::make_shared<Actor>(gameboard.get()); // Make actor and parent it to gameboard
	enemies[1]->AddComponent<ShaderComponent>(shader); // Add shader
	enemies[1]->AddComponent<MaterialComponent>(enemyTexture); // Add texture
	enemies[1]->AddComponent<AiComponent>(enemies[1].get()); // Add AI component
	enemies[1]->AddComponent<MeshComponent>(e); // Add mesh
	enemies[1]->AddComponent<PhysicsComponent>(nullptr, Vec3(5.0f, 0.0f, 0.05f), // Different position
		QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.75f, 0.75f, 0.75f));

	// Add both enemies to the actor list
	AddActor(enemies[0]);
	AddActor(enemies[1]);
}
//
//void Scene1::OrientCharacterToCamera()
//{
//	// Get positions
//	Vec3 mariosPos = character->GetComponent<PhysicsComponent>()->GetPosition();
//	locationManager.mariosPos = mariosPos;
//	Vec3 cameraPos = camera->GetComponent<TransformComponent>()->GetPosition();
//	
//	// Calculate direction vector
//	Vec3 direction = cameraPos - mariosPos;
//
//	// Normalize the direction
//	direction = VMath::normalize(direction);
//
//	// Define the up vector (typically the world's up)
//	Vec3 up(0.0f, 1.0f, 0.0f);
//
//	// Compute the orientation quaternion to look at the camera
//	Quaternion orientation = QMath::lookAt(direction, up);
//
//	// Apply the orientation to the character
//	character->GetComponent<TransformComponent>()->SetTransform(mariosPos, orientation);
//}
//

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


