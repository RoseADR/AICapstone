#include "ScottCollisionComponent.h"
#include <glew.h> 
#include "cmath"


CollisionComponent::CollisionComponent(Component* parent_, ColliderType type_, float radius_, Vec3 normal_, float dist_):
Component(parent_), colliderType(type_), radius(radius_), normal(normal_), dist(dist_) {

}

