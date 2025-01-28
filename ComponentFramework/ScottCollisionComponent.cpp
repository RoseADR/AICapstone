#include "CollisionComponent.h"
#include "ScottCollisionComponent.h"

SFCollisionComponent::SFCollisionComponent(Component* parent_, ColliderType type_, float radius_ ):
		Component(parent_),colliderType(type_), radius(radius_) {

}
