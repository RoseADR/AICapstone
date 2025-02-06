#include <glew.h>
#include "BillBoardedCameraActor.h"
#include "MMath.h"
#include "QMath.h"
#include "TransformComponent.h"
#include "Debug.h"
#include "UBO_Padding.h"

using namespace MATH;

// BillBoardCameraActor constructor
BillBoardCameraActor::BillBoardCameraActor(Component* parent_)
    : Actor(parent_), bindingPoint(0), uboMatriciesID(0) {
    // Initializes with default values
}

// OnCreate: Called to initialize the camera actor
bool BillBoardCameraActor::OnCreate() {
    // If already created, don't redo creation
    if (isCreated) return isCreated;

    Debug::Info("Creating values for BillBoardCameraActor: ", __FILE__, __LINE__);

    // Set up a buffer size for matrix data, twice the size of a 4x4 matrix
    size_t buffer_size = 2 * UBO_PADDING::MAT4;

    // Generate and bind a new uniform buffer object (UBO) for matrices
    glGenBuffers(1, &uboMatriciesID);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatriciesID);
    glBufferData(GL_UNIFORM_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);  // Allocate buffer data
    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // Unbind the UBO

    // Bind the buffer to a specific binding point (for shader usage)
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboMatriciesID);

    // Update the camera's projection matrix (perspective)
    UpdateProjectionMatrix(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);  // Default values for FOV, aspect ratio, and near/far plane
    UpdateViewMatrix();  // Update the view matrix (camera's position and orientation)
    return isCreated;
}

// UpdateViewMatrix: Recalculates the view matrix based on the camera's position and orientation
void BillBoardCameraActor::UpdateViewMatrix() {
    // Get the transform component of the actor
    Ref<TransformComponent> transformComponent = GetComponent<TransformComponent>();

    // If there is no transform component, use a default view matrix
    if (transformComponent.get() == nullptr) {
        viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        // If transform component exists, get its orientation and position
        Quaternion orientation = transformComponent->GetQuaternion();
        Vec3 position = transformComponent->GetPosition();

        // Create transformation matrices for position and orientation
        Matrix4 trans = MMath::translate(position);  // Translation matrix
        Matrix4 orient = MMath::toMatrix4(orientation);  // Orientation matrix

        // Combine the two matrices to get the view matrix
        viewMatrix = orient * trans;
        viewMatrix.print("ViewMatrix");  // Print the view matrix for debugging
    }

    // Update the raycast matrix (for proper picking in 3D space)
    UpdateRayCastMatrix();

    // Update the UBO with the new view matrix
    size_t offset = sizeof(Matrix4);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatriciesID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Matrix4), viewMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// UpdateProjectionMatrix: Recalculates the projection matrix (perspective projection)
void BillBoardCameraActor::UpdateProjectionMatrix(const float fovy, const float aspectRatio, const float near, const float far) {
    size_t offset = 0;

    // Generate the perspective projection matrix
    projectionMatrix = MMath::perspective(fovy, aspectRatio, near, far);

    // Update the raycast matrix with the new projection matrix
    UpdateRayCastMatrix();

    // Update the UBO with the new projection matrix
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatriciesID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Matrix4), projectionMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// UpdateRayCastMatrix: Calculates a matrix to transform from screen space to world space for raycasting
void BillBoardCameraActor::UpdateRayCastMatrix() {
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);  // Get the current viewport dimensions
    // Generate the raycast matrix by transforming the NDC space using viewport, projection, and view matrix
    rayCastMatrix = MMath::inverse(MMath::viewportNDC(viewport[2], viewport[3]) * projectionMatrix * viewMatrix);
}

// Destructor: Cleanup when the object is destroyed
BillBoardCameraActor::~BillBoardCameraActor() {
    OnDestroy();  // Clean up resources
}

// OnDestroy: Cleanup any OpenGL buffers and reset state
void BillBoardCameraActor::OnDestroy() {
    glDeleteBuffers(1, &uboMatriciesID);  // Delete the UBO buffer
    isCreated = false;  // Mark as destroyed
}