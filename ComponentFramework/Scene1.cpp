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
	Quaternion orientation = QMath::angleAxisRotation(-45.0f, Vec3(1.0f, 0.0f, 0.0f));

	gameboard->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.0f),orientation);
	gameboard->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	gameboard->AddComponent<ShaderComponent>(shader);
	gameboard->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("ChessBoard"));
	AddActor(gameboard);

	character = std::make_shared<Actor>(gameboard.get());
	Quaternion mariosQuaternion = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f) * QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)));

	character->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 2.5f), mariosQuaternion);
	character->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Mario"));
	character->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("MarioMain"));
	character->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	AddActor(character);

	LoadEnemies();

	return true;
}

Scene1::~Scene1() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
	OnDestroy();
}

void Scene1::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);
}

void Scene1::HandleEvents(const SDL_Event &sdlEvent) {
	static int objID = -1;
	static Vec2 currentMousePos;
	static Vec2	lastMousePos;
	static float flip = 1.0f;
	Ref<TransformComponent> cameraTC;
	Ref<TransformComponent> characterTC;
	Ref<TransformComponent> gameBoardTC;

	Vec3 rotatedDirection;
	Vec3 direction;  
	Quaternion start;
	Quaternion end;

	/// Handle Camera movement 
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		cameraTC = camera->GetComponent<TransformComponent>();
		characterTC = character->GetComponent<TransformComponent>();
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
		
			characterTC->SetTransform(characterTC->GetPosition() + Vec3(0.0f, 0.1f, 0.0f), characterTC->GetQuaternion());
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
	
			characterTC->SetTransform(characterTC->GetPosition() + Vec3(-0.1f, 0.0f, 0.0f), start = characterTC->GetQuaternion());
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
	
			characterTC->SetTransform(characterTC->GetPosition() + Vec3(0.0f, -0.1f, 0.0f), start = characterTC->GetQuaternion());
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
	
			characterTC->SetTransform(characterTC->GetPosition() + Vec3(0.1f, 0.0f, 0.0f), start = characterTC->GetQuaternion());
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

	case SDL_MOUSEMOTION:
	{
		if (objID < 0) break;
		currentMousePos = Vec2(static_cast<float>(sdlEvent.button.x), static_cast<float>(sdlEvent.button.y));
		Vec3 pos = actors[objID]->GetComponent<TransformComponent>()->GetPosition();
		if (currentMousePos.y - lastMousePos.y > 0.0f) {
			pos.y -= 0.08f * flip;
			pos.z = 0.2f;
		} else {
			pos.y += 0.08f * flip;
			pos.z = 0.2f;
		}

		actors[objID]->GetComponent<TransformComponent>()->SetPosition(pos);
		lastMousePos = currentMousePos;
	}
	break;

	case SDL_MOUSEBUTTONDOWN:

		currentMousePos = Vec2(static_cast<float>(sdlEvent.button.x), static_cast<float>(sdlEvent.button.y));
		lastMousePos = currentMousePos;
		objID = Pick(sdlEvent.button.x, sdlEvent.button.y);
		printf("0x%X %d\n", objID, objID);
		break;

	case SDL_MOUSEBUTTONUP: 
	{
		if (objID == -1) break;
		float integerPart;
		Vec3 pos = actors[objID]->GetComponent<TransformComponent>()->GetPosition();
		/// Snap to a grid square
		integerPart = round(pos.y);
		pos.y = -3.5f + ((integerPart - -4.0f) * 1.0f);
		/// Lower the piece 
		pos.z = 0.0f;
		actors[objID]->GetComponent<TransformComponent>()->SetPosition(pos);
		objID = -1;
		break;
	}
	default:
		break;
    }
}

void Scene1::Update(const float deltaTime) {
	Ref<TransformComponent> characterTC;
	Ref<TransformComponent> enemyTC;

	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);
	locationManager.mariosPos = character->GetComponent<TransformComponent>()->GetPosition();
	Vec3 mePos = actors[2]->GetComponent<TransformComponent>()->GetPosition();
	Vec3 move = actors[2]->GetComponent<AiComponent>()->Flee(mePos, locationManager.mariosPos);

	//Vec3 move = actors[2]->GetComponent<AiComponent>()->Flee(mePos, locationManager.mariosPos);

	//REMEBER
	enemyTC = actors[2]->GetComponent<TransformComponent>();
	enemyTC->SetTransform(enemyTC->GetPosition() + move * deltaTime, enemyTC->GetQuaternion());

	//locationManager.mariosPos.print();

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
	

	Ref<Actor> enemy;

	
		enemy = std::make_shared<Actor>(gameboard.get()); // makes actor and parents it to gameboard
		enemy->AddComponent<ShaderComponent>(shader);// add shader
		enemy->AddComponent<MaterialComponent>(enemyTexture);// add texture
		enemy->AddComponent<AiComponent>(enemy.get());
		enemy->AddComponent<MeshComponent>(e);// add mesh
		enemy->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.05f),
			QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));

		enemies = std::make_shared<Actor>(gameboard.get());
		enemies->AddComponent<ShaderComponent>(shader);
		enemies->AddComponent<MaterialComponent>(enemyTexture);

			enemies->AddComponent<MeshComponent>(e);
			enemies->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
		AddActor(enemy);// adds to the actor list
			
		
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


