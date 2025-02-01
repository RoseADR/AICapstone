//#include "CollisionComponent.h"
//#include "VMath.h"
//#include <iostream>
//#include <glew.h>
//
//CollisionComponent::CollisionComponent(Actor* parent_, ColliderShape shape_, const Vec3& size_, float radius_, const Vec3& offset_)
//    : Component(parent_), shape(shape_), size(size_), radius(radius_), offset(offset_) {
//    transform = parent_->GetComponent<TransformComponent>().get();
//    if (!transform) {
//        std::cerr << "[ERROR] CollisionComponent: TransformComponent not found in parent!" << std::endl;
//    }
//}
//
//
//
//Vec3 CollisionComponent::GetMinBounds() const {
//    Vec3 position = transform->GetPosition() + offset;
//    return position - size * 0.5f;
//    //    Matrix4 transformMatrix = transform->GetTransformMatrix();
////    return transformMatrix * Vec4(-size * 0.5f + offset, 1.0f);
//}
//
//Vec3 CollisionComponent::GetMaxBounds() const {
//    Vec3 position = transform->GetPosition() + offset;
//    return position + size * 0.5f;
//    //    Matrix4 transformMatrix = transform->GetTransformMatrix();
////    return transformMatrix * Vec4(size * 0.5f + offset, 1.0f);
//}
//
//
//CollisionComponent::~CollisionComponent() {
//    OnDestroy();
//}
//
//bool CollisionComponent::OnCreate() {
//    if (!transform) {
//        std::cerr << "[ERROR] TransformComponent not found for CollisionComponent!" << std::endl;
//        return false;
//    }
//    return true;
//}
//
//void CollisionComponent::OnDestroy() {
//    // Cleanup logic (if necessary)
//}
//
//void CollisionComponent::Update(const float deltaTime_) {
//    if (!transform) return;
//
//    Vec3 position = transform->GetPosition();
//    Vec3 minBounds = GetMinBounds();
//    Vec3 maxBounds = GetMaxBounds();
//
//  /*  std::cout << "[CollisionComponent] Actor Position: ("
//        << position.x << ", " << position.y << ", " << position.z << ")\n";
//    std::cout << "[CollisionComponent] Transform Matrix:\n" << transformMatrix << "\n";
//
//    //Vec3 position = transform->GetPosition();
//    Vec3 minBounds = GetMinBounds();
//    Vec3 maxBounds = GetMaxBounds();
//
//    std::cout << "[CollisionComponent] Collision Box Min: ("
//        << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << ")\n";
//    std::cout << "[CollisionComponent] Collision Box Max: ("
//        << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << ")\n";*/
//}

//void CollisionComponent::Render() const {
//    if (!transform) return;
//
//    Vec3 minBounds = GetMinBounds();
//    Vec3 maxBounds = GetMaxBounds();
//
//    // Apply transformation (fix visual offset issues)
//    Matrix4 modelMatrix = transform->GetTransformMatrix();
//    minBounds = modelMatrix * Vec4(minBounds, 1.0f);
//    maxBounds = modelMatrix * Vec4(maxBounds, 1.0f);
//
//    glDisable(GL_TEXTURE_2D);
//    glEnable(GL_LINE_SMOOTH);
//    glLineWidth(1.0f);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    glColor3f(1.0f, 0.0f, 0.0f); // Red for collision debugging
//
//    glBegin(GL_LINES);
//     Bottom face
//    glVertex3f(minBounds.x, minBounds.y, minBounds.z); glVertex3f(maxBounds.x, minBounds.y, minBounds.z);
//    glVertex3f(maxBounds.x, minBounds.y, minBounds.z); glVertex3f(maxBounds.x, minBounds.y, maxBounds.z);
//    glVertex3f(maxBounds.x, minBounds.y, maxBounds.z); glVertex3f(minBounds.x, minBounds.y, maxBounds.z);
//    glVertex3f(minBounds.x, minBounds.y, maxBounds.z); glVertex3f(minBounds.x, minBounds.y, minBounds.z);
//
//     Top face
//    glVertex3f(minBounds.x, maxBounds.y, minBounds.z); glVertex3f(maxBounds.x, maxBounds.y, minBounds.z);
//    glVertex3f(maxBounds.x, maxBounds.y, minBounds.z); glVertex3f(maxBounds.x, maxBounds.y, maxBounds.z);
//    glVertex3f(maxBounds.x, maxBounds.y, maxBounds.z); glVertex3f(minBounds.x, maxBounds.y, maxBounds.z);
//    glVertex3f(minBounds.x, maxBounds.y, maxBounds.z); glVertex3f(minBounds.x, maxBounds.y, minBounds.z);
//
//     Vertical edges
//    glVertex3f(minBounds.x, minBounds.y, minBounds.z); glVertex3f(minBounds.x, maxBounds.y, minBounds.z);
//    glVertex3f(maxBounds.x, minBounds.y, minBounds.z); glVertex3f(maxBounds.x, maxBounds.y, minBounds.z);
//    glVertex3f(maxBounds.x, minBounds.y, maxBounds.z); glVertex3f(maxBounds.x, maxBounds.y, maxBounds.z);
//    glVertex3f(minBounds.x, minBounds.y, maxBounds.z); glVertex3f(minBounds.x, maxBounds.y, maxBounds.z);
//    glEnd();
//
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glDisable(GL_LINE_SMOOTH);
//}
//
//
//
//bool CollisionComponent::CheckCollision(const CollisionComponent& other) const {
//    Vec3 thisMin = transform->GetPosition() - size * 0.5f; // Min corner
//    Vec3 thisMax = transform->GetPosition() + size * 0.5f; // Max corner
//
//    Vec3 otherMin = other.transform->GetPosition() - other.size * 0.5f;
//    Vec3 otherMax = other.transform->GetPosition() + other.size * 0.5f;
//
//    return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
//        (thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
//        (thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
//}
//
