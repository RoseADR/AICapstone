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


using namespace MATH;


Scene1::Scene1():drawNormals(false),drawOverlay(false) {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
}

bool Scene1::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
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
	orientationBoard = QMath::angleAxisRotation(-45.0f, Vec3(1.0f, 0.0f, 0.0f));

	gameboard->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.0f), orientationBoard);
	gameboard->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	gameboard->AddComponent<ShaderComponent>(shader);
	gameboard->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("ChessBoard"));
	AddActor(gameboard);

	character = std::make_shared<Actor>(gameboard.get());
	Quaternion mariosQuaternion = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f) * QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)));

	character->AddComponent<PhysicsComponent>(nullptr, Vec3(0.0f, 10.0f, 2.5f), mariosQuaternion);
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
	std::vector<Node*> path = graph->findPath(sceneNodes[0], sceneNodes[63]);

	// Placeholder: Use path as needed for movement or rendering



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
}

void Scene1::HandleEvents(const SDL_Event &sdlEvent) {
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

		case SDL_SCANCODE_SPACE:

			flip *= -1.0f;
			start = gameBoardTC->GetQuaternion();
			end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			gameBoardTC->SlerpOrientation(start, end, 3.0f);
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

		case SDL_SCANCODE_W:
		{
			Vec3 Position = characterTC->GetPosition();
			characterTC->SetPosition(Position + Vec3(0.0f, 1.0f, 0.0f));
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

			characterTC->SetPosition(characterTC->GetPosition() + Vec3(-0.1f, 0.0f, 0.0f));
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

			characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.0f, -0.1f, 0.0f));
			/*flip *= -1.0f;
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

			characterTC->SetPosition(characterTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f));
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
	Ref<PhysicsComponent> characterTC;
	Ref<PhysicsComponent> enemyTC;
	Ref<PhysicsComponent> enemyTC2;

	Vec3 mariosPos = character->GetComponent<PhysicsComponent>()->GetPosition();
	Vec3 mariosVel = character->GetComponent<PhysicsComponent>()->getVel();

	// Update the gameboard transform
	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);

	// Get Mario's position (character's position)
	locationManager.mariosPos = character->GetComponent<PhysicsComponent>()->GetPosition();

	// --- Enemy 1 (actors[2]) Flee ---
	Vec3 enemy1Pos = actors[2]->GetComponent<PhysicsComponent>()->GetPosition();
	Vec3 enemy1Move = actors[2]->GetComponent<AiComponent>()->Flee(enemy1Pos, locationManager.mariosPos);

	// Update Enemy 1's position
	enemyTC = actors[2]->GetComponent<PhysicsComponent>();
	enemyTC->SetTransform(enemyTC->GetPosition() + enemy1Move * deltaTime, enemyTC->GetQuaternion());

	// --- Enemy 2 (actors[3]) Follow ---
	Vec3 enemy2Vel = actors[3]->GetComponent<PhysicsComponent>()->getVel();
	Vec3 enemy2Pos = actors[3]->GetComponent<PhysicsComponent>()->GetPosition();
	//Vec3 enemy2Move = actors[3]->GetComponent<AiComponent>()->Pursuit(enemy2Pos, mariosPos, enemy2Vel);
	Vec3 enemy2Move = actors[3]->GetComponent<AiComponent>()->Arrive(enemy2Pos, mariosPos);

	// Update Enemy 2's position
	enemyTC2= actors[3]->GetComponent<PhysicsComponent>();
	enemyTC2->SetTransform(enemyTC2->GetPosition() + enemy2Move * deltaTime, enemyTC2->GetQuaternion());

	// Debug or print Mario's position if needed
	// locationManager.mariosPos.print();
}


void Scene1::Render() const {

	glEnable(GL_DEPTH_TEST);
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

	
	// Render each tile in the grid
	for (int i = 0; i < tiles.size(); i++) {
		for (int j = 0; j < tiles[i].size(); j++) {
			tiles[i][j]->render();
		}
	}

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
	enemies[0]->AddComponent<PhysicsComponent>(nullptr, Vec3(0.0f, 0.0f, 0.05f),
		QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));

	// Set up the second enemy
	enemies[1] = std::make_shared<Actor>(gameboard.get()); // Make actor and parent it to gameboard
	enemies[1]->AddComponent<ShaderComponent>(shader); // Add shader
	enemies[1]->AddComponent<MaterialComponent>(enemyTexture); // Add texture
	enemies[1]->AddComponent<AiComponent>(enemies[1].get()); // Add AI component
	enemies[1]->AddComponent<MeshComponent>(e); // Add mesh
	enemies[1]->AddComponent<PhysicsComponent>(nullptr, Vec3(1.0f, 0.0f, 0.05f), // Different position
		QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.75f, 0.75f, 0.75f));

	// Add both enemies to the actor list
	AddActor(enemies[0]);
	AddActor(enemies[1]);
}

// Creates an 8x8 grid of tiles and initializes nodes
void Scene1::createTiles() {
	int gridSize = 8;

	//technically these should be equal to make a square however the render is not showing as a square, 
	//Might have to change manually until they equal a square. Or could be a different issue
	//could also potentially effect the pathfinding
	float tileWidth = 0.15f;  
	float tileHeight = 0.15f; 

	graph = new Graph();
	tiles.resize(gridSize, std::vector<Tile*>(gridSize));
	sceneNodes.resize(gridSize * gridSize);

	Quaternion orientationTile = QMath::angleAxisRotation(-45.0f, Vec3(1.0f, 0.0f, 0.0f));

	int label = 0;
	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			Node* node = new Node(label);
			sceneNodes[label] = node;

			// Position each tile in a grid layout
			Vec3 tilePos = Vec3( -.45 + (j * tileWidth), -.45 + (i * tileHeight), 0.0f); // the number before is the offset from center
			//Vec3 rotatedTilePos = QMath::rotate(tilePos, orientationTile);
			Tile* tile = new Tile(node, tilePos, tileWidth, tileHeight, this);
			
			tiles[i][j] = tile;

			// Debug print for tile width, height, and position
			std::cout << "Tile (" << i << ", " << j << ") - "
				<< "Width: " << tileWidth << ", Height: " << tileHeight << ", "
				<< "Position: (" << tilePos.x << ", " << tilePos.y << ", " << tilePos.z << ")\n";
				//<< "RotatedPosition: (" << rotatedTilePos.x << ", " << rotatedTilePos.y << ", " << rotatedTilePos.z << ")\n";

			label++;
		}
	}

	// Initialize the graph with the list of nodes
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


