#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene2.h"
#include "MMath.h"
#include "Debug.h"
#include "CameraActor.h"
#include "LightActor.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "MaterialComponent.h"
#include "MMath.h"
#include <string>
using namespace MATH;


Scene2::Scene2():drawNormals(false),drawOverlay(false) {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

bool Scene2::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	assetManager = std::make_shared<AssetManager>();
	
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -13.0f), Quaternion());
	camera->OnCreate();
	camera->GetProjectionMatrix().print("ProjectionMatrix");
	camera->GetViewMatrix().print("ViewMatrix");

	
	light = std::make_shared<LightActor>(camera.get(), LightStyle::DirectionLight, Vec3(0.0f, 5.0f, 1.0f),Vec4(0.85f,0.6,0.6f,0.0f));
	light->OnCreate();

	

	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	gameboard = std::make_shared<Actor>(nullptr);
	Quaternion orientation = QMath::angleAxisRotation(-60.0f, Vec3(1.0f, 0.0f, 0.0f));
	gameboard->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.0f), orientation);
	gameboard->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Plane"));
	gameboard->AddComponent<ShaderComponent>(shader);
	gameboard->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("ChessBoard"));
	gameboard->OnCreate();
	AddActor(gameboard);

	LoadBlackPieces();
	LoadWhitePieces();
	
	return true;
}





Scene2::~Scene2() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
	OnDestroy();
}

void Scene2::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);
}

void Scene2::HandleEvents(const SDL_Event &sdlEvent) {
	static int objID = -1;
	static Vec2 currentMousePos;
	static Vec2	lastMousePos;
	static float flip = 1.0f;
	Ref<TransformComponent> cameraTC;
	Ref<TransformComponent> gameBoardTC;

	/// Handle Camera movement 
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		cameraTC = camera->GetComponent<TransformComponent>();
		switch (sdlEvent.key.keysym.scancode) {
		

		case SDL_SCANCODE_LEFT:
		{
			Quaternion rotate = cameraTC->GetQuaternion() * QMath::angleAxisRotation(-1.0f, Vec3(0.0f, 1.0f, 0.0f));
			cameraTC->SetTransform(cameraTC->GetPosition(), rotate);
			camera->UpdateViewMatrix();
			light->UpdateUBO();
			break;
		}
		
		case  SDL_SCANCODE_RIGHT:
		{
			Quaternion rotate = cameraTC->GetQuaternion() * QMath::angleAxisRotation(1.0f, Vec3(0.0f, 1.0f, 0.0f));
			cameraTC->SetTransform(cameraTC->GetPosition(), rotate);
			camera->UpdateViewMatrix();
			light->UpdateUBO();
			break;
		}

		case SDL_SCANCODE_UP:
			cameraTC->SetTransform(cameraTC->GetPosition() + Vec3(0.0f, 0.0f, 0.1f), cameraTC->GetQuaternion());
			camera->UpdateViewMatrix();
			break;

		case SDL_SCANCODE_DOWN:
			cameraTC->SetTransform(cameraTC->GetPosition() + Vec3(0.0f, 0.0f, -0.1f), cameraTC->GetQuaternion());
			camera->UpdateViewMatrix();
			break;

		case SDL_SCANCODE_SPACE:
		{
			flip *= -1.0f;
			gameBoardTC = gameboard->GetComponent<TransformComponent>();
			Quaternion start = gameBoardTC->GetQuaternion();
			Quaternion end = QMath::angleAxisRotation(180.0f * flip, Vec3(0.0f, 0.0f, 1.0f)) * start;
			gameBoardTC->SlerpOrientation(start, end, 3.0f);
			break;
		}
		}


		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_E:
			cameraTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion() *
				QMath::angleAxisRotation(-2.0f, Vec3(0.0f, 1.0f, 0.0f)));
			break;

		case SDL_SCANCODE_Q:
			cameraTC->SetTransform(cameraTC->GetPosition(), cameraTC->GetQuaternion() *
				QMath::angleAxisRotation(2.0f, Vec3(0.0f, 1.0f, 0.0f)));
			break;

		case SDL_SCANCODE_W:
			break;

		case SDL_SCANCODE_A:
			break;

		case SDL_SCANCODE_S:
			break;

		case SDL_SCANCODE_D:
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
		/// Snap to a the grid square
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

void Scene2::Update(const float deltaTime) {
	gameboard->GetComponent<TransformComponent>()->Update(deltaTime);
}

void Scene2::Render() const {
	glEnable(GL_DEPTH_TEST);
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

	

void Scene2::DrawNormals(const Vec4 color) const {
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




int Scene2::Pick(int x, int y) {
	glEnable(GL_DEPTH_TEST);
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

void Scene2::DrawMeshOverlay(const Vec4 color) const {
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

void Scene2::LoadBlackPieces() {
	Ref<MeshComponent> p = assetManager->GetComponent<MeshComponent>("Pawn");
	Ref<MeshComponent> r = assetManager->GetComponent<MeshComponent>("Rook");
	Ref<MeshComponent> kn = assetManager->GetComponent<MeshComponent>("Knight");
	Ref<MeshComponent> b = assetManager->GetComponent<MeshComponent>("Bishop");
	Ref<MeshComponent> q = assetManager->GetComponent<MeshComponent>("Queen");
	Ref<MeshComponent> k = assetManager->GetComponent<MeshComponent>("King");
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	Ref<MaterialComponent> bChessTexture = assetManager->GetComponent<MaterialComponent>("BlackChessTexture");

	Ref<Actor> actor;
	
	float yPos = 3.5f;
	float xPos = -3.5f;
	for (int j = 0; j < 8; j++) {
		actor = std::make_shared<Actor>(gameboard.get());
		actor->AddComponent<ShaderComponent>(shader);
		actor->AddComponent<MaterialComponent>(bChessTexture);
		switch (j) {
		case 0:
			actor->AddComponent<MeshComponent>(r);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;
		case 1:
			actor->AddComponent<MeshComponent>(kn);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)) *
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)),
				Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 2:
			actor->AddComponent<MeshComponent>(b);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 3:
			actor->AddComponent<MeshComponent>(q);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 4:
			actor->AddComponent<MeshComponent>(k);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 5:
			actor->AddComponent<MeshComponent>(b);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 6:
			actor->AddComponent<MeshComponent>(kn);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(0.0f, Vec3(0.0f, 1.0f, 0.0f)) *
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)),
				Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 7:
			actor->AddComponent<MeshComponent>(r);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;
		default:
			break;
		}
		xPos += 1.0f;
	}
	xPos = -3.5f;
	yPos -= 1.0f;
	for (int j = 0; j < 8; j++) {
		actor = std::make_shared<Actor>(gameboard.get());
		actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
			QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
		actor->AddComponent<MeshComponent>(p);
		actor->AddComponent<ShaderComponent>(shader);
		actor->AddComponent<MaterialComponent>(bChessTexture);
		actor->OnCreate();
		AddActor(actor);
		xPos += 1.0f;
	}
}
void Scene2::LoadWhitePieces() {
	Ref<MeshComponent> p = assetManager->GetComponent<MeshComponent>("Pawn");
	Ref<MeshComponent> r = assetManager->GetComponent<MeshComponent>("Rook");
	Ref<MeshComponent> kn = assetManager->GetComponent<MeshComponent>("Knight");
	Ref<MeshComponent> b = assetManager->GetComponent<MeshComponent>("Bishop");
	Ref<MeshComponent> q = assetManager->GetComponent<MeshComponent>("Queen");
	Ref<MeshComponent> k = assetManager->GetComponent<MeshComponent>("King");
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("TextureShader");
	Ref<MaterialComponent> wChessTexture = assetManager->GetComponent<MaterialComponent>("WhiteChessTexture");

	Ref<Actor> actor;
	float yPos = -3.5f;
	float xPos = -3.5f;
	for (int j = 0; j < 8; j++) {
		actor = std::make_shared<Actor>(gameboard.get());
		actor->AddComponent<ShaderComponent>(shader);
		actor->AddComponent<MaterialComponent>(wChessTexture);
		switch (j) {
		case 0:
			actor->AddComponent<MeshComponent>(r);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;
		case 1:
			actor->AddComponent<MeshComponent>(kn);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) * 
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)),
				Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 2:
			actor->AddComponent<MeshComponent>(b);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 3:
			actor->AddComponent<MeshComponent>(q);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 4:
			actor->AddComponent<MeshComponent>(k);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 5:
			actor->AddComponent<MeshComponent>(b);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 6:
			actor->AddComponent<MeshComponent>(kn);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f)) * 
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)),
				Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;

		case 7:
			actor->AddComponent<MeshComponent>(r);
			actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
				QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
			AddActor(actor);
			break;
		default:
			break;
		}
		xPos += 1.0f;
	}

	xPos = -3.5f;
	yPos += 1.0f;
	for (int j = 0; j < 8; j++) {
		actor = std::make_shared<Actor>(gameboard.get());
		actor->AddComponent<TransformComponent>(nullptr, Vec3(xPos, yPos, -0.05f),
			QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.15f, 0.15f, 0.15f));
		actor->AddComponent<MeshComponent>(p);
		actor->AddComponent<ShaderComponent>(shader);
		actor->AddComponent<MaterialComponent>(wChessTexture);
		actor->OnCreate();
		AddActor(actor);
		xPos += 1.0f;
	}

}