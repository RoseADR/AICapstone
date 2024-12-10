#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene0.h"
#include "MMath.h"
#include "Debug.h"
#include "CameraActor.h"
#include "LightActor.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "MaterialComponent.h"
#include "MMath.h"

#include <chrono>
#include <thread>

using namespace MATH;


Scene0::Scene0():drawNormals(false),drawOverlay(false){

	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
}

bool Scene0::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	///The create the asset manager
	assetManager = std::make_shared<AssetManager>();
	
	
	camera = std::make_shared<CameraActor>(nullptr);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -10.0f), Quaternion());
	camera->OnCreate();
	camera->GetProjectionMatrix().print("ProjectionMatrix");
	

	light = std::make_shared<LightActor>(camera.get(), LightStyle::DirectionLight, Vec3(-10.0f, 0.0f, 0.0f) ,Vec4(0.6f,0.6f,0.6f,0.0f));
	light->OnCreate();
	
	
	Ref<Actor> someObject = std::make_shared<Actor>(nullptr);
	Quaternion orientation = QMath::angleAxisRotation(180.0f, Vec3(0.0f, 1.0f, 0.0f));
	someObject->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.0f), orientation);

	someObject->AddComponent<MeshComponent>(assetManager->GetComponent<MeshComponent>("Mario"));
	someObject->AddComponent<ShaderComponent>(assetManager->GetComponent<ShaderComponent>("TextureShader"));
	someObject->AddComponent<MaterialComponent>(assetManager->GetComponent<MaterialComponent>("MarioMain"));

	AddActor(someObject);

	return true;
}

Scene0::~Scene0() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
	OnDestroy();
}

void Scene0::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);

	

}

void Scene0::HandleEvents(const SDL_Event &sdlEvent) {
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
			light->UpdateUBO();
			break;

		case SDL_SCANCODE_DOWN:
			cameraTC->SetTransform(cameraTC->GetPosition() + Vec3(0.0f, 0.0f, -0.1f), cameraTC->GetQuaternion());
			camera->UpdateViewMatrix();
			light->UpdateUBO();
			break;

		case SDL_SCANCODE_SPACE:
			break;

		}

		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_E:
		{
			if (objID < 0) break;
			Ref<TransformComponent> pickedObjTransform = actors[objID]->GetComponent<TransformComponent>();
			//Quaternion q = pickedObjTransform->GetQuaternion();
			pickedObjTransform->SetTransform(pickedObjTransform->GetPosition(), pickedObjTransform->GetQuaternion() *
				QMath::angleAxisRotation(2.0f, Vec3(0.0f, 1.0f, 0.0f)));
		}
			break;

		case SDL_SCANCODE_Q:
		{
			if (objID < 0) break;
			Ref<TransformComponent> pickedObjTransform = actors[objID]->GetComponent<TransformComponent>();
			//Quaternion q = pickedObjTransform->GetQuaternion();
			pickedObjTransform->SetTransform(pickedObjTransform->GetPosition(), pickedObjTransform->GetQuaternion() *
				QMath::angleAxisRotation(-2.0f, Vec3(0.0f, 1.0f, 0.0f)));
		}
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
			pos.y -= 0.08f;
			pos.z = 0.2f;
		} else {
			pos.y += 0.08f;
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
		if (objID == -1) break;
		objID = -1; /// Let go of the object
		break;

	default:
		break;
    }
}

void Scene0::Update(const float deltaTime) {
}

void Scene0::Render() const {
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
		DrawMeshOverlay(Vec4(1.0f, 1.0f, 1.0f, 0.0f));
	}

	if (drawNormals == true) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.1f));
	}
}

void Scene0::DrawNormals(const Vec4 color) const {
	glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatriciesID());
	Ref<ShaderComponent> shader = assetManager->GetComponent<ShaderComponent>("DrawNormalsShader");
	glUseProgram(shader->GetProgram());
	glUniform4fv(shader->GetUniformID("color"), 1, color);
	for (auto actor : actors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
		actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
	}
	glUseProgram(0);
}

void Scene0::DrawMeshOverlay(const Vec4 color) const {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_LINE_SMOOTH);
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

int Scene0::Pick(int x, int y) {
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); /// Paint the backgound white which is 0x00FFFFFF
	glClear(GL_COLOR_BUFFER_BIT);
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
