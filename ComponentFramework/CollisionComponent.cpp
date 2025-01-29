#include "CollisionComponent.h"
#include "VMath.h"
#include <iostream>
#include <glew.h>

CollisionComponent::CollisionComponent(Actor* parent_, ColliderShape shape_, const Vec3& size_, float radius_, const Vec3& offset_)
    : Component(parent_), shape(shape_), size(size_), radius(radius_), offset(offset_) {
    transform = parent_->GetComponent<TransformComponent>().get();
    if (!transform) {
        std::cerr << "[ERROR] CollisionComponent: TransformComponent not found in parent!" << std::endl;
    }
}

Vec3 CollisionComponent::GetMinBounds() const {
    Vec3 position = transform->GetPosition() + offset;
    return position - size * 0.5f;
}

Vec3 CollisionComponent::GetMaxBounds() const {
    Vec3 position = transform->GetPosition() + offset;
    return position + size * 0.5f;
}
//
//Vec3 CollisionComponent::GetMinBounds() const {
//    Vec3 position = transform->GetPosition() + offset;
//    Vec3 minBounds = position - size * 0.5f;
//
//    //std::cout << "Min Bounds: " << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << std::endl;
//    return minBounds;
//}
//
//Vec3 CollisionComponent::GetMaxBounds() const {
//    Vec3 position = transform->GetPosition() + offset;
//    Vec3 maxBounds = position + size * 0.5f;
//
//    //std::cout << "Max Bounds: " << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << std::endl;
//    return maxBounds;
//}


CollisionComponent::~CollisionComponent() {
    OnDestroy();
}

bool CollisionComponent::OnCreate() {
    if (!transform) {
        std::cerr << "[ERROR] TransformComponent not found for CollisionComponent!" << std::endl;
        return false;
    }
    return true;
}

void CollisionComponent::OnDestroy() {
    // Cleanup logic (if necessary)
}

void CollisionComponent::Update(const float deltaTime_) {
    if (!transform) return;

    Vec3 position = transform->GetPosition();
    Vec3 minBounds = GetMinBounds();
    Vec3 maxBounds = GetMaxBounds();

    std::cout << "[CollisionComponent] Actor Position: ("
        << position.x << ", " << position.y << ", " << position.z << ")\n";
    std::cout << "[CollisionComponent] Collision Box Min: ("
        << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << ")\n";
    std::cout << "[CollisionComponent] Collision Box Max: ("
        << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << ")\n";
}

void CollisionComponent::Render() const {
    if (!transform) return;

    Vec3 minBounds = GetMinBounds();
    Vec3 maxBounds = GetMaxBounds();

    glDisable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Render in wireframe mode
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for debugging

    glBegin(GL_LINES);
    // Draw box edges
    // Bottom square
    glVertex3f(minBounds.x, minBounds.y, minBounds.z);
    glVertex3f(maxBounds.x, minBounds.y, minBounds.z);

    glVertex3f(maxBounds.x, minBounds.y, minBounds.z);
    glVertex3f(maxBounds.x, minBounds.y, maxBounds.z);

    glVertex3f(maxBounds.x, minBounds.y, maxBounds.z);
    glVertex3f(minBounds.x, minBounds.y, maxBounds.z);

    glVertex3f(minBounds.x, minBounds.y, maxBounds.z);
    glVertex3f(minBounds.x, minBounds.y, minBounds.z);

    // Top square
    glVertex3f(minBounds.x, maxBounds.y, minBounds.z);
    glVertex3f(maxBounds.x, maxBounds.y, minBounds.z);

    glVertex3f(maxBounds.x, maxBounds.y, minBounds.z);
    glVertex3f(maxBounds.x, maxBounds.y, maxBounds.z);

    glVertex3f(maxBounds.x, maxBounds.y, maxBounds.z);
    glVertex3f(minBounds.x, maxBounds.y, maxBounds.z);

    glVertex3f(minBounds.x, maxBounds.y, maxBounds.z);
    glVertex3f(minBounds.x, maxBounds.y, minBounds.z);

    // Vertical edges
    glVertex3f(minBounds.x, minBounds.y, minBounds.z);
    glVertex3f(minBounds.x, maxBounds.y, minBounds.z);

    glVertex3f(maxBounds.x, minBounds.y, minBounds.z);
    glVertex3f(maxBounds.x, maxBounds.y, minBounds.z);

    glVertex3f(maxBounds.x, minBounds.y, maxBounds.z);
    glVertex3f(maxBounds.x, maxBounds.y, maxBounds.z);

    glVertex3f(minBounds.x, minBounds.y, maxBounds.z);
    glVertex3f(minBounds.x, maxBounds.y, maxBounds.z);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



bool CollisionComponent::CheckCollision(const CollisionComponent& other) const {
    Vec3 thisMin = transform->GetPosition() - size * 0.5f; // Min corner
    Vec3 thisMax = transform->GetPosition() + size * 0.5f; // Max corner

    Vec3 otherMin = other.transform->GetPosition() - other.size * 0.5f;
    Vec3 otherMax = other.transform->GetPosition() + other.size * 0.5f;

    return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
        (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
        (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
}

