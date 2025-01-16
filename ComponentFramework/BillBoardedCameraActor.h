#pragma once
#include "Actor.h"
#include "Matrix.h"
using namespace MATH;

class BillBoardCameraActor:public Actor {
	/// Unless you know what these do don't allow them
	/// to be created implicitly 
	BillBoardCameraActor(const BillBoardCameraActor&) = delete;
	BillBoardCameraActor(BillBoardCameraActor&&) = delete;
	BillBoardCameraActor& operator=(const BillBoardCameraActor&) = delete;
	BillBoardCameraActor& operator=(BillBoardCameraActor&&) = delete;

private:
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	/// This is experimental for an Umer project: ray casting and ray tracing 
	Matrix4 rayCastMatrix;

	unsigned int uboMatriciesID;
	const GLuint bindingPoint; /// This must be unique to the camera = 0, lights = 1
public:
	BillBoardCameraActor(Component* parent_);
	virtual ~BillBoardCameraActor();

	bool OnCreate() override;
	void OnDestroy() override;
	Matrix4 GetProjectionMatrix() const { return projectionMatrix; }
	Matrix4 GetViewMatrix() const { return viewMatrix; }

	void UpdateProjectionMatrix(const float fovy, const float aspectRatio, const float near, const float far);
	void UpdateViewMatrix();
	void UpdateRayCastMatrix();
	GLuint GetMatriciesID() const { return uboMatriciesID; }
};

