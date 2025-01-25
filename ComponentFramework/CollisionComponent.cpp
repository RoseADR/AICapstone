#include "CollisionComponent.h"
#include "VMath.h"
#include <iostream>

CollisionComponent::CollisionComponent(Actor* parent_, ColliderShape shape_, const Vec3& size_, float radius_, const Vec3& offset_)
    : Component(parent_), shape(shape_), size(size_), radius(radius_), offset(offset_) {
    transform = parent_->GetComponent<TransformComponent>().get();
    if (!transform) {
        std::cerr << "[ERROR] CollisionComponent: TransformComponent not found in parent!" << std::endl;
    }
}

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
    // Example: Update the position of the collider based on the parent's transform
    if (transform) {
        // Sync collider's position or bounds
    }
}

void CollisionComponent::Render() const {
    // This might not need to render anything for a collider
    // Add debug visualization here, if desired
    std::cout << "Rendering collision bounds (debug view)" << std::endl;
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

