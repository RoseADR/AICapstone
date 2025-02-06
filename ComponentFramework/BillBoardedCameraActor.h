#pragma once
#include "Actor.h"
#include "Matrix.h"
using namespace MATH;

class BillBoardCameraActor : public Actor {
    // Prevents copying and moving this class to avoid unintended behavior
    // If you're not sure how to manage these, don't allow implicit creation
    BillBoardCameraActor(const BillBoardCameraActor&) = delete; // Disable copy constructor
    BillBoardCameraActor(BillBoardCameraActor&&) = delete;      // Disable move constructor
    BillBoardCameraActor& operator=(const BillBoardCameraActor&) = delete; // Disable copy assignment
    BillBoardCameraActor& operator=(BillBoardCameraActor&&) = delete;      // Disable move assignment

private:
    Matrix4 projectionMatrix; // Stores the camera's projection matrix
    Matrix4 viewMatrix;       // Stores the camera's view matrix

    // This is experimental for ray casting and ray tracing in the Umer project
    Matrix4 rayCastMatrix;    // Stores the ray casting matrix

    unsigned int uboMatriciesID; // Uniform buffer object ID for storing matrix data
    const GLuint bindingPoint;   // Unique binding point for the camera (0), lights (1), etc.

public:
    // Constructor initializing the parent component
    BillBoardCameraActor(Component* parent_);

    // Destructor
    virtual ~BillBoardCameraActor();

    // Called when the camera is created, initializes necessary resources
    bool OnCreate() override;

    // Called when the camera is destroyed, releases resources
    void OnDestroy() override;

    // Returns the projection matrix
    Matrix4 GetProjectionMatrix() const { return projectionMatrix; }

    // Returns the view matrix
    Matrix4 GetViewMatrix() const { return viewMatrix; }

    // Updates the projection matrix with given parameters
    void UpdateProjectionMatrix(const float fovy, const float aspectRatio, const float near, const float far);

    // Updates the view matrix based on the camera's position and orientation
    void UpdateViewMatrix();

    // Updates the ray casting matrix for the experimental project
    void UpdateRayCastMatrix();

    // Returns the ID of the uniform buffer object containing the matrices
    GLuint GetMatriciesID() const { return uboMatriciesID; }
};

