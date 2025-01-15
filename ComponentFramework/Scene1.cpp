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

#include "CollisionSystem.h"

#include <chrono>
#include <thread>

// test test delete me 
using namespace MATH;


Scene1::Scene1():engine(nullptr), drawNormals(false), drawOverlay(false), controller(nullptr){
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
}

bool Scene1::OnCreate() {
	
	//Timing timing("Scene1::OnCreate");

	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
	assetManager = std::make_shared<AssetManager>();
	
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -13.0f), Quaternion());
	camera->OnCreate();
	camera->GetProjectionMatrix().print("ProjectionMatrix");
	camera->GetViewMatrix().print("ViewMatrix");

	light = std::make_shared<LightActor>(nullptr, LightStyle::DirectionLight, Vec3(3.0f, 5.0f, -5.0f),Vec4(0.9f,0.9f,0.9f,0.0f));
	light->OnCreate();

	
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	gameboard = std::make_shared<Actor>(nullptr);
	orientationBoard = QMath::angleAxisRotation(-60.0f, Vec3(1.0f, 0.0f, 0.0f));

	gameboard->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 1.0f, 0.0f), orientationBoard);
	gameboard->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	gameboard->AddComponent<ShaderComponent>(shader);
	gameboard->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BlackChessTexture"));
	AddActor(gameboard);

	
	Vec3 boardPos =  actors[0]->GetComponent<TransformComponent>()->GetPosition();
	//std::cout << "GameBoard Position: ("
		/*<< boardPos.x << ", "
		<< boardPos.y << ", "
		<< boardPos.z << ")\n"*/;


	character = std::make_shared<Actor>(gameboard.get());
	Quaternion mariosQuaternion = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f) * QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)));

	character->AddComponent<PhysicsComponent>(nullptr, Vec3(0.0f, 0.0f, 4.5f), mariosQuaternion);
	character->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Mario"));
	character->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("MarioMain"));
	character->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	AddActor(character);

	LoadEnemies();

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

	controller = SDL_JoystickOpen(0);
	

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

		/*case SDL_SCANCODE_SPACE:

			flip *= -1.0f;
			start = gameBoardTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			gameBoardTC->SlerpOrientation(start, end, 3.0f);
			break;*/


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

		case SDL_SCANCODE_W:
		{
			//characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.0f, 0.0f));
			orientationU= QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)); // Facing upward
			characterTC->SetTransform(characterTC->GetPosition(), orientationU);
			break;
		}

		case SDL_SCANCODE_B: { // Check if 'G' key is pressed
			showTiles = !showTiles; // Toggle the visibility flag
		}
			/*flip *= -1.0f;
			start = characterTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			characterTC->SlerpOrientation(start, end, 2.0f);
			characterTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion());*/
			//characterTC->GetPosition() + Vec3(0.0f, 0.1f, 0.0f);

			/*direction = Vec3(0.0f, 1.0f, 0.0f);
			rotatedDirection = characterTC->GetQuaternion() * direction;
			characterTC->SetTransform(characterTC->GetPosition() + rotatedDirection * 0.1f, characterTC->GetQuaternion());*/

			break;


		case SDL_SCANCODE_A:

			//characterTC->SetPosition(characterTC->GetPosition() + Vec3(-0.0f, 0.0f, 0.0f));
			orientationL = QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f)) *   // Turn left
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
			characterTC->SetTransform(characterTC->GetPosition(), orientationL);
			/*flip *= -1.0f;
			start = characterTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			characterTC->SlerpOrientation(start, end, 2.0f);
			characterTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion());*/
			//characterTC->GetPosition() + Vec3(-0.1f, 0.0f, 0.0f);

			/*direction = Vec3(-1.0f, 0.0f, 0.0f);
			rotatedDirection = characterTC->GetQuaternion() * direction;
			characterTC->SetTransform(characterTC->GetPosition() + rotatedDirection * 0.1f, characterTC->GetQuaternion());*/
			break;


		case SDL_SCANCODE_S:

			//characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, -0.0f, 0.0f));
			orientationD = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn to face backward
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));     
			characterTC->SetTransform(characterTC->GetPosition(), orientationD);
			/*flip *= -1.0f
			start = characterTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			characterTC->SlerpOrientation(start, end, 2.0f);
			characterTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion());*/
			//	characterTC->GetPosition() + Vec3(0.0f, 0.1f, 0.0f);
			/*direction = Vec3(0.0f, -1.0f, 0.0f);
			rotatedDirection = characterTC->GetQuaternion() * direction;
			characterTC->SetTransform(characterTC->GetPosition() + rotatedDirection * 0.1f, characterTC->GetQuaternion());*/
			break;


		case SDL_SCANCODE_D:

			//characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.0f, 0.0f));
			orientationR = QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn right
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
			characterTC->SetTransform(characterTC->GetPosition(), orientationR);
			/*flip *= -1.0f;
			start = characterTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			characterTC->SlerpOrientation(start, end, 2.0f);
			characterTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion());*/
			//characterTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f);

		/*	direction = Vec3(1.0f, 0.0f, 0.0f);
			rotatedDirection = characterTC->GetQuaternion() * direction;
			characterTC->SetTransform(characterTC->GetPosition() + rotatedDirection * 0.1f, characterTC->GetQuaternion());*/
			break;

		case SDL_SCANCODE_R:

			characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.0f, -0.1f));
			orientationR = QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn right
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
			characterTC->SetTransform(characterTC->GetPosition(), orientationR);
			/*flip *= -1.0f;
			start = characterTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			characterTC->SlerpOrientation(start, end, 2.0f);
			characterTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion());*/
			//characterTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f);

		/*	direction = Vec3(1.0f, 0.0f, 0.0f);
			rotatedDirection = characterTC->GetQuaternion() * direction;
			characterTC->SetTransform(characterTC->GetPosition() + rotatedDirection * 0.1f, characterTC->GetQuaternion());*/
			break;

		case SDL_SCANCODE_T:

			characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, 0.0f, 0.1f));
			orientationR = QMath::angleAxisRotation(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *  // Turn right
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));    // Stay upright
			characterTC->SetTransform(characterTC->GetPosition(), orientationR);
			/*flip *= -1.0f;
			start = characterTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			characterTC->SlerpOrientation(start, end, 2.0f);
			characterTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion());*/
			//characterTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f);

		/*	direction = Vec3(1.0f, 0.0f, 0.0f);
			rotatedDirection = characterTC->GetQuaternion() * direction;
			characterTC->SetTransform(characterTC->GetPosition() + rotatedDirection * 0.1f, characterTC->GetQuaternion());*/
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

	//Was trying to calculate the players velocity here when we change positions to use in the pursue and arrive
	//// Calculate velocity as (new position - previous position) / deltaTime
	//if (deltaTime > 0.0f) { // Prevent division by zero
	//	Vec3 newPosition = characterTC->GetPosition();
	//	velPlayer = (newPosition - previousPlayerPos) / deltaTime;

	//	// Update previous position to the new position
	//	previousPlayerPos = newPosition;
	//}
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

	const float gravity = -9.8f;        // Gravitational acceleration
	const float jumpStrength = 5.0f;   // Initial jump velocity
	const float moveSpeed = 3.7f;      // Movement speed
	static float verticalVelocity = 0.0f; // Character's vertical velocity
	

	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);
	auto characterTransform = character->GetComponent<TransformComponent>();

	if (characterTransform) {
		Vec3 pos = characterTransform->GetPosition();

		// Check if the character is on the floor
		isGrounded = CollisionHandler::CheckCollision(character, gameboard);

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
			CollisionHandler::ResolveCollision(character, gameboard);
		}
		else {
			// Apply gravity when not grounded
			verticalVelocity += gravity * deltaTime;
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

	/*Vec3 axis(xAxis, yAxis, 0.0f);
	if (VMath::mag(axis) > 1) {
		characterTC->SetPosition(characterTC->GetPosition() + VMath::normalize(axis) * 0.1);
	}*/
	
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
		actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	//glPushMatrix();
	
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); /// Paint the backgound white which is 0x00FFFFFF
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


