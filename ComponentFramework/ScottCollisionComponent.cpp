#include "ScottCollisionComponent.h"


CollisionComponent::CollisionComponent(Component* parent_, ColliderType type_, float radius_) :
	Component(parent_), colliderType(type_), radius(radius_) {

}
