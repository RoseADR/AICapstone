#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "Actor.h"

enum class ColliderShape {
    Sphere,
    AABB,
    OBB
};

class CollisionComponent : public Component {
private:
    ColliderShape shape;
    Vec3 size;       // Dimensions for AABB/OBB
    float radius;    // Radius for sphere
    Vec3 offset;     // Offset from parent center
    TransformComponent* transform; // Reference to the parent transform

public:
    CollisionComponent(Actor* parent_, ColliderShape shape_, const Vec3& size_ = Vec3(10.0f, 10.0f, 1.0f), float radius_ = 0.5f, const Vec3& offset_ = Vec3(0.0f, 0.0f, 0.0f));
    ~CollisionComponent();

    // Implementing Component's pure virtual methods
    bool OnCreate() override;
    void OnDestroy() override;
    void Update(const float deltaTime_) override;
    void Render() const override;

    // Accessors
    ColliderShape GetShape() const { return shape; }
    Vec3 GetSize() const { return size; }
    float GetRadius() const { return radius; }
    Vec3 GetOffset() const { return offset; }

    // Utility methods
    bool CheckCollision(const CollisionComponent& other) const;
};
