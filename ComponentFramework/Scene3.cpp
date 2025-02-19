#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene3.h"
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


Scene3::Scene3() :engine(nullptr), drawNormals(false), drawOverlay(false) {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);

}

bool Scene3::OnCreate() {

	

	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
	assetManager = std::make_shared<AssetManager>();
	orientationCam = QMath::angleAxisRotation(5.0f, Vec3(1.0f, 0.0f, 0.0f));
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -7.0f, -13.0f), orientationCam);
	camera->OnCreate();
	camera->GetProjectionMatrix().print("ProjectionMatrix");
	camera->GetViewMatrix().print("ViewMatrix");

	light = std::make_shared<LightActor>(nullptr, LightStyle::DirectionLight, Vec3(1.0f, 0.0f, 0.0f), Vec4(0.9f, 0.9f, 0.9f, 0.0f));
	light->OnCreate();


	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	Ref<CollisionComponent> cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::Sphere, 1.0f);
	Ref<PhysicsComponent> pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(0.0f, 0.0f, 4.1f), orientationBoard);

	
	gameboard = std::make_shared<Actor>(nullptr);

	orientationBoard = QMath::angleAxisRotation(276.0f, Vec3(1.0f, 0.0f, 0.0f));
	pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(0.0f, -10.0f, -10.0f), orientationBoard);
	cc = std::make_shared<CollisionComponent>(nullptr, ColliderType::PLANE, 0.0f, Vec3(0.0f, -1.0f, 0.0f));
	//gameboard->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.1f, -10.0f), orientationBoard, Vec3(1.0, 1.0, 1.0));
	gameboard->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	gameboard->AddComponent<ShaderComponent>(shader);
	gameboard->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("road"));
	gameboard->AddComponent(cc);
	gameboard->AddComponent(pc);
	// <CollisionComponent>(nullptr,
	//	ColliderType::Sphere, Vec3(9.0f, 2.0f, 22.5f), // Width, height, depth
	//	0.0f, Vec3(-5.0f, 0.0f, 0.0f)); // Offset
	// 
	gameboard->OnCreate();
	AddActor(gameboard);




	

	


		character = std::make_shared<Actor>(nullptr);
		Quaternion mariosQuaternion = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 0.0f, 1.0f)) * QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) * QMath::angleAxisRotation(180.0f, Vec3(1.0f, 0.0f, 0.0f));
		pc = std::make_shared<PhysicsComponent>(nullptr, Vec3(-15.0f, 10.0f, -10.0f), mariosQuaternion);
		character->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
		character->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("RoboGun"));
		character->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("Billboard"));
		character->AddComponent(cc);
		character->AddComponent(pc);
		// <CollisionComponent>(nullptr, 
		//	ColliderType::Sphere, /*Vec3(1.0f, 1.0f, 2.0f), */
		//	0.0f/*, Vec3(0.0f, 0.5f, 0.0f))*/);
		character->OnCreate();
		AddActor(character);

		/*projectile = std::make_shared<Actor>(character.get());
		projectile->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Cube"));
		projectile->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BulletSkin"));
		projectile->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));*/


		LoadEnemies();

		collisionSystem.AddActor(gameboard);
		collisionSystem.AddActor(character);
		

		physicsSystem.AddActor(character);
		physicsSystem.AddActor(gameboard);


	
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

void Scene3::FireProjectile() {
	// Get the character's transform
	auto charTransform = character->GetComponent<TransformComponent>();
	if (!charTransform) return;

	Vec3 charPos = charTransform->GetPosition(); // Start position on the character

	auto projectile = std::make_shared<Actor>(nullptr);
	projectile->AddComponent<TransformComponent>(nullptr, charPos, Quaternion());
	projectile->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Cube"));
	projectile->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("BulletSkin"));
	projectile->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));

	// Gravity-enabled physics
	Vec3 gravity(0.0f, -9.81f, 0.0f); // Falling downward
	projectile->AddComponent<PhysicsComponent>(nullptr, charPos, Quaternion(), Vec3(0.0f, 0.0f, 0.0f), gravity);

	projectile->OnCreate();
	AddActor(projectile);
	projectiles.push_back(projectile);
}


void Scene3::HandleEvents(const SDL_Event& sdlEvent) {
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

	bool facingRight = false; // animation
	bool facingLeft = false;

	/// Handle Camera movement 


	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		if (hackingMode) {
			int newX = hackingPlayerPos.x;
			int newY = hackingPlayerPos.y;

			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_W:
				if (hackingPlayerPos.y < hackingTiles.size() - 1) newY++;
				break;
			case SDL_SCANCODE_S:
				if (hackingPlayerPos.y > 0) newY--;
				break;
			case SDL_SCANCODE_A:
				if (hackingPlayerPos.x > 0) newX--;
				break;
			case SDL_SCANCODE_D:
				if (hackingPlayerPos.x < hackingTiles[0].size() - 1) newX++;
				break;
			case SDL_SCANCODE_SPACE:
				if (hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->getNode()->getIsBlocked()) {
					hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->getNode()->setIsBlocked(false);
				}
				break;
			}

			// Update player position
			if (newX != hackingPlayerPos.x || newY != hackingPlayerPos.y) {
				// Reset the old tile
				hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->setPathTile(false);

				// Move player
				hackingPlayerPos.x = newX;
				hackingPlayerPos.y = newY;

				// Mark new tile as player's position
				hackingTiles[hackingPlayerPos.y][hackingPlayerPos.x]->setPathTile(true);
			}
		}
		break;
	}


	switch (sdlEvent.type) {


	case SDL_MOUSEBUTTONDOWN:
		if (!hackingMode) {
			if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
				FireProjectile();

			}
		}
		break;



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

		case SDL_SCANCODE_L:
			showHackingGrid = !showHackingGrid;
			hackingMode = showHackingGrid; // Toggle hacking mode
			if (showHackingGrid && hackingTiles.empty()) {
				createHackingGrid();
			}
			break;



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


	}

}

void Scene3::Update(const float deltaTime) {
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

	for (size_t i = 0; i < projectiles.size();) {
		auto projectile = projectiles[i];

		auto transform = projectile->GetComponent<TransformComponent>();
		auto physics = projectile->GetComponent<PhysicsComponent>();

		if (!transform || !physics) {
			projectiles.erase(projectiles.begin() + i);
			continue;
		}

		// Apply gravity
		//will have to make x negative when facing left will do it later
		if (facing) {
			Vec3 newVel = physics->getVel() + Vec3(4.0f, -9.8f * deltaTime, 0.0f);

			physics->SetVelocity(newVel);

			// Move the projectile
			transform->SetPosition(transform->GetPosition() + newVel * deltaTime);

			// Remove if it falls below a threshold (e.g., off-screen)
			if (transform->GetPosition().y < -50.0f) {
				projectiles.erase(projectiles.begin() + i);

			}
			else {
				++i;
			}
		}

		if (!facing) {
			Vec3 newVel = physics->getVel() + Vec3(-4.0f, -9.8f * deltaTime, 0.0f);

			physics->SetVelocity(newVel);

			// Move the projectile
			transform->SetPosition(transform->GetPosition() + newVel * deltaTime);

			// Remove if it falls below a threshold (e.g., off-screen)
			if (transform->GetPosition().y < -50.0f) {
				projectiles.erase(projectiles.begin() + i);
			}
			else {
				++i;
			}
		}
	}


	for (auto actor : actors) {
		auto transform = actor->GetComponent<TransformComponent>();
		if (transform) {
			Vec3 pos = transform->GetPosition();
			/*std::cout << "[Scene1] Actor Position Updated: ("
				<< pos.x << ", " << pos.y << ", " << pos.z << ")\n";*/
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
	const float moveSpeed = 30.0f;      // Movement speed
	static float verticalVelocity = 0.0f; // Character's vertical velocity

	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);
	auto characterTransform = character->GetComponent<TransformComponent>();


	/*if (characterTransform) {
		bool p = true;
		p = collisionSystem.SphereSphereCollisionDetection(character, TestCube);

		if (p) {
			collisionSystem.SphereSphereCollisionResponse(character, TestCube);
		}
	}

	*/


	//if (characterTransform) {
	//	Vec3 pos = characterTransform->GetPosition();
	//	Ref<PhysicsComponent> playerPhysics = character->GetComponent<PhysicsComponent>();
	//	Ref<CollisionComponent> playerCollision = character->GetComponent<CollisionComponent>();

	//	// Check collision with ground (gameboard)
	//	Ref<CollisionComponent> groundCollision = gameboard->GetComponent<CollisionComponent>();
	//	Ref<PhysicsComponent> groundPhysics = gameboard->GetComponent<PhysicsComponent>();


	//	/*if (playerPhysics && playerCollision ) {
	//		Sphere playerSphere;
	//		playerSphere.r = playerCollision->GetRadius();
	//		playerSphere.center = character->GetComponent<TransformComponent>()->GetPosition();

	//		Sphere groundSphere;
	//		groundSphere.r = groundCollision->GetRadius();
	//		groundSphere.center = gameboard->GetComponent<TransformComponent>()->GetPosition();*/

	//	for (auto actor : actors) {
	//		auto collider = actor->GetComponent<CollisionComponent>();
	//		if (collider || actor == character) {

	//			Sphere playerSphere;
	//			playerSphere.r = playerCollision->GetRadius();
	//			playerSphere.center = character->GetComponent<TransformComponent>()->GetPosition();

	//			Plane groundPlane;
	//			groundPlane = gameboard->GetComponent<CollisionComponent>()->GetPlane();


	//			Sphere otherSphere;
	//			otherSphere.r = collider->GetRadius();
	//			otherSphere.center = actor->GetComponent<TransformComponent>()->GetPosition();


	//			if (isGrounded) {

	//				//Reset vertical velocity when grounded
	//				verticalVelocity = 0.0f;

	//				// Jump if space is pressed
	//				const Uint8* keystate = SDL_GetKeyboardState(nullptr);
	//				if (keystate[SDL_SCANCODE_SPACE]) {
	//					verticalVelocity = jumpStrength; // Apply jump velocity
	//					isGrounded = false;              // Character is no longer grounded
	//				}
	//				// Resolve collision to align with the floor
	//				if (isGrounded) {
	//					collisionSystem.SpherePlaneCollisionResponse(playerSphere, playerPhysics, groundPlane, groundPhysics);
	//				}

	//				if (collisionSystem.SpherePlaneCollisionDetection(playerSphere, groundPlane)) {
	//					isGrounded = true;
	//					collisionSystem.SpherePlaneCollisionResponse(playerSphere, playerPhysics, groundPlane, groundPhysics /*actor->GetComponent<PhysicsComponent>()*/);
	//				}

	//			}

	//			if (!isGrounded) {
	//				Vec3 currentVel = playerPhysics->getVel();
	//				currentVel.z += -9.8f * deltaTime;
	//				playerPhysics->SetVelocity(currentVel);
	//			}
	//			else {
	//				Vec3 currentPos = playerTransform->GetPosition();
	//				if (currentPos.z < otherSphere.center.z + otherSphere.r) {
	//					currentPos.z = otherSphere.center.z + otherSphere.r; // makes sure it stays on the ground
	//					playerTransform->SetPosition(currentPos);
	//				}
	//			}
	//		}



		// Handle horizontal motion (WASD input)
	Vec3 pos = characterTransform->GetPosition();
	Vec3 horizontalMove(0.0f, 0.0f, 0.0f); // Movement direction
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);
	if (!hackingMode) {
		if (keystate[SDL_SCANCODE_W]) horizontalMove.y += 1.0f; // Forward
		if (keystate[SDL_SCANCODE_S]) horizontalMove.y -= 1.0f; // Backward
		if (keystate[SDL_SCANCODE_A]) facingLeft = true, // last part for animation
			facing = false, horizontalMove.x -= 1.0f; // Left
		if (keystate[SDL_SCANCODE_D]) facingRight = true,
			facing = true, horizontalMove.x += 1.0f; // Right
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








	Ref<TransformComponent>characterTC = character->GetComponent<TransformComponent>();
	Ref<PhysicsComponent> enemyTC;


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

	// animation movement
	if (facingRight) {
		currentTime += deltaTime;
		index = static_cast <int> (currentTime / frameSpeed) % 8;
		//std::cout << index << std:: endl;
		if (facingLeft) {
			currentTime += deltaTime;
			index = static_cast <int> (currentTime / frameSpeed) % 8;
		}
	}

	collisionSystem.Update(deltaTime);
	physicsSystem.Update(deltaTime);

	// Update the gameboard transform
	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);
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
		glUniform1f(actor->GetComponent<ShaderComponent>()->GetUniformID("index"), index);
		actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);

	}

	glDisable(GL_BLEND);

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

	for (auto actor : actors) {
		auto collider = actor->GetComponent<CollisionComponent>();
		if (collider) {
			collider->Render(); // Render collision boxes
		}
	}
	////glPushMatrix();

	if (showHackingGrid) {
		for (const auto& row : hackingTiles) {
			for (Tile* tile : row) {
				tile->render();
			}
		}
	}

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

	for (const auto& projectile : projectiles) {
		projectile->Render();
	}
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

void Scene3::LoadEnemies() {
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



// Sets up connections (left, right, up, down) between adjacent nodes


void Scene3::createHackingGrid() {
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

	std::vector<std::pair<int, int>> redTilePositions;
	int numRedTiles = (gridSize * gridSize) / 4; // 25% of tiles will be red
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


