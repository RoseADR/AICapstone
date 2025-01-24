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
    if (shape == ColliderShape::Sphere && other.GetShape() == ColliderShape::Sphere) {
        // Sphere-sphere collision
        Vec3 centerA = transform->GetPosition() + offset;
        Vec3 centerB = other.transform->GetPosition() + other.GetOffset();
        float combinedRadius = radius + other.GetRadius();
        return VMath::mag(centerA - centerB) <= combinedRadius;
        printf("fjff");
    }
    // Add other collision logic (e.g., AABB vs AABB, Sphere vs AABB)
    return false; // Default: no collision
}

