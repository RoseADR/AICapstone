#include "ScottCollisionSystem.h"
#include <PMath.h>


void CollisionSystem::Update(const float deltaTime) {
    for (size_t i = 0; i < collidingActors.size(); ++i) {
        for (size_t j = i + 1; j < collidingActors.size(); ++j) {

            Sphere s1, s2;  /// I'm just going to do sphere-sphere collions first
            s1.r = collidingActors[i]->GetComponent<CollisionComponent>()->radius;
            s1.center = collidingActors[i]->GetComponent<PhysicsComponent>()->pos;

            s2.r = collidingActors[j]->GetComponent<CollisionComponent>()->radius;
            s2.center = collidingActors[j]->GetComponent<PhysicsComponent>()->pos;

            if (SphereSphereCollisionDetection(s1, s2) == true) {
                Ref<PhysicsComponent> pc1 = collidingActors[i]->GetComponent<PhysicsComponent>();
                Ref<PhysicsComponent> pc2 = collidingActors[j]->GetComponent<PhysicsComponent>();
                SphereSphereCollisionResponse(s1, pc1, s2, pc2);
            }
        }
    }
}


bool CollisionSystem::SphereSphereCollisionDetection(const Sphere& s1, const Sphere& s2) const {
    float distance = VMath::distance(s1.center, s2.center);
    if (distance < s1.r + s2.r) {
        return true;
    }
    return false;
}

bool CollisionSystem::AABBAABBCollisionDetection(const AABB& bb1, const AABB& bb2) const {
    if (abs(bb1.center.x - bb2.center.x) > bb1.rx + bb2.rx) return false;
    if (abs(bb1.center.y - bb2.center.y) > bb1.ry + bb2.ry) return false;
    if (abs(bb1.center.z - bb2.center.z) > bb1.rz + bb2.rz) return false;
    return true;
}

bool CollisionSystem::SphereAABBCollisionDetection(const Sphere& s1, const Sphere& s2) const
{

    return true;
}

/// Not yet tested but the math is lifted from my earlier working code - SSF
void CollisionSystem::SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2) {
    const float e = 0.8f; // Coefficient of restitution (elasticity)
    Vec3 L = s1.center - s2.center;
    float distance = VMath::mag(L);
    float radiusSum = s1.r + s2.r;

    // If no overlap, return
    if (distance >= radiusSum) return;

    // Normalize collision normal
    Vec3 n = VMath::normalize(L);

    // Separate spheres using Minimum Translation Vector (MTV)
    float penetrationDepth = radiusSum - distance;
    float totalMass = pc1->mass + pc2->mass;

    if (totalMass > 0.0f) {
        float correctionFactor = penetrationDepth / totalMass;
        pc1->pos += n * (correctionFactor * pc2->mass);
        pc2->pos -= n * (correctionFactor * pc1->mass);
    }

    // Relative velocity
    Vec3 v1 = pc1->vel;
    Vec3 v2 = pc2->vel;
    float v1p = VMath::dot(v1, n);
    float v2p = VMath::dot(v2, n);

    // If already separating, return
    if (v1p - v2p > 0.0f) return;

 
    // Compute new velocities after collision
    float v1p_new = ((pc1->mass - e * pc2->mass) * v1p + (1.0f + e) * pc2->mass * v2p) / totalMass;
    float v2p_new = ((pc2->mass - e * pc1->mass) * v2p + (1.0f + e) * pc1->mass * v1p) / totalMass;

    // Apply new velocities along the collision normal
    pc1->vel = v1 + (v1p_new - v1p) * n;
    pc2->vel = v2 + (v2p_new - v2p) * n;

}

void CollisionSystem::AABBAABBCollisionResponse(AABB bb1, Ref<PhysicsComponent> pc1, AABB bb2, Ref<PhysicsComponent> pc2)
{
}

void CollisionSystem::SphereAABBCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, AABB bb2, Ref<PhysicsComponent> pc2)
{
}


bool CollisionSystem::SpherePlaneCollisionDetection(const Sphere& s1, const Plane& p1)
{
 /*   float dist = PMath::distance(s1.center, p1);
   
    if (dist > s1.r) {
        return true;
        std::cout << "Collision with Plane Detected" << std::endl;
    }*/
    return true;

}
