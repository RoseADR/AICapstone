#include "ScottCollisionSystem.h"
#include <PMath.h>
#include <MMath.h>

using namespace MATH;

void CollisionSystem::Update(const float deltaTime) {

    Ref<PhysicsComponent> pc1 = character->GetComponent<PhysicsComponent>();
    Ref<TransformComponent> tc1 = character->GetComponent<TransformComponent>();
    Ref<CollisionComponent> cc1 = character->GetComponent<CollisionComponent>();
    if (!cc1 || (!pc1 && !tc1)) return;

    Sphere s1;
    s1.r = cc1->GetRadius();
    s1.center = pc1 ? pc1->pos : tc1->GetPosition();

    Vec3 obj1Pos = pc1 ? pc1->pos : tc1->GetPosition();

    // Factory bounds
    Vec3 factoryPos = factory->GetComponent<TransformComponent>()->GetPosition();
    float factoryX = factoryPos.x;
    float factoryThreshold = 30.0f;

    // Bridge bounds 
    Vec3 bridgePos = bridge->GetComponent<TransformComponent>()->GetPosition();
    float bridgeX = bridgePos.x;
    float bridgeThreshold = 10.0f;

    for (size_t i = 0; i < collidingActors.size(); ++i) {
        Ref<PhysicsComponent> pc2 = collidingActors[i]->GetComponent<PhysicsComponent>();
        Ref<TransformComponent> tc2 = collidingActors[i]->GetComponent<TransformComponent>();
        Ref<CollisionComponent> cc2 = collidingActors[i]->GetComponent<CollisionComponent>();
        if (!cc2 || (!pc2 && !tc2)) continue;

        Vec3 obj2Pos = pc2 ? pc2->pos : tc2->GetPosition();

        // Factory Bounds
        bool withinFactoryBounds = !( obj1Pos.x < factoryX - factoryThreshold || obj1Pos.x > factoryX + factoryThreshold ||
            obj2Pos.x < factoryX - factoryThreshold || obj2Pos.x > factoryX + factoryThreshold);

        // Bridge Bounds
        bool withinBridgeBounds = !(obj1Pos.x < bridgeX - bridgeThreshold || obj1Pos.x > bridgeX + bridgeThreshold ||
            obj2Pos.x < bridgeX - bridgeThreshold || obj2Pos.x > bridgeX + bridgeThreshold);
        
       
        if (cc2->GetColliderType() == ColliderType::Sphere) {
            Sphere s2;
            s2.r = cc2->radius;
            s2.center = pc2 ? pc2->pos : tc2->GetPosition();

            if (SphereSphereCollisionDetection(s1, s2)) {
                SphereSphereCollisionResponse(s1, pc1, s2, pc2);
                std::cout << "Collision detected with shpere actor: " << i << std::endl;
            }
        }
        else if (cc2->GetColliderType() == ColliderType::PLANE) {
            Plane p;
            p.n = cc2->normal;
            p.d = cc2->dist; 
            //p.d = VMath::dot(p.d, tc2->GetPosition());

            //std::cout << "Bridge: " << withinBridgeBounds << std::endl;
            //std::cout << "factory: " << withinFactoryBounds << std::endl;

            if (SpherePlaneCollisionDetection(s1, p)) {
                if (withinFactoryBounds || withinBridgeBounds) {
                    SpherePlaneCollisionResponse(s1, pc1, p);
                    std::cout << "Collision with plane actor: " << i << std::endl;
                }
            }
        }
    }
}

bool CollisionSystem::SphereSphereCollisionDetection(const Sphere& s1, const Sphere& s2) const {
    float distance = VMath::distance(s1.center, s2.center);
    if (distance < s1.r + s2.r) {
        std::cout << "SphereSphere Collision" << std::endl;
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

    // If no overlap or if the centers are nearly identical, return to avoid division by zero
    if (distance < VERY_SMALL || distance >= radiusSum) return;

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

    // Impulse scalar
    float j = -(1 + e) * (v1p - v2p) / (1 / pc1->mass + 1 / pc2->mass);

    // Apply impulse
    Vec3 impulse = j * n;
    if (totalMass > 0.0f) {
        float correctionFactor = penetrationDepth / totalMass;

       
        pc1->pos += n * penetrationDepth; 
       
        // pc2->pos -= n * 0.01f; 
    }

}


void CollisionSystem::AABBAABBCollisionResponse(AABB bb1, Ref<PhysicsComponent> pc1, AABB bb2, Ref<PhysicsComponent> pc2)
{
}

void CollisionSystem::SphereAABBCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, AABB bb2, Ref<PhysicsComponent> pc2)
{
}


bool CollisionSystem::SpherePlaneCollisionDetection(const Sphere& s1, const Plane& p1)
{
    float dist = VMath::dot(s1.center, p1.n) - p1.d;

    if (fabs(dist <= s1.r)) {
        //  std::cout << "SpherePlane Collision" << std::endl;
        return true;
    }
  
}

void CollisionSystem::SpherePlaneCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Plane p2/*, Ref<PhysicsComponent> tc2*/)
{
   
    float dist = VMath::dot(s1.center, p2.n) - p2.d;

    //// If the sphere is penetrating the plane, apply MTV
    //if (dist < s1.r) {
    //    float penetrationDepth = s1.r - dist;  // Overlap amount

    //    // Move the sphere completely out of the plane
    //    pc1->pos += p2.n * penetrationDepth;

    //    // Stop downward movement
    //    float velocityAlongNormal = VMath::dot(pc1->vel, p2.n);
    //    if (velocityAlongNormal < 0.0f) {
    //        pc1->vel -= p2.n * velocityAlongNormal;  // Remove movement in the plane normal direction
    //    }
    //}
  /*  std::cout << "Sphere-Plane collision: dist=" << dist
        << " | penetration=" << s1.r - dist
        << " | pos=" << pc1->pos
        << " | vel=" << pc1->vel << std::endl;*/

    if (dist < s1.r) {
        float penetrationDepth = s1.r - dist;

        //  Clamp the sphere just outside the plane
        pc1->pos += p2.n * penetrationDepth;

        //  Completely stop velocity into the plane
        float velocityAlongNormal = VMath::dot(pc1->vel, p2.n);
        if (velocityAlongNormal < 0.0f) {
            pc1->vel -= p2.n * velocityAlongNormal;

            //  Bonus: zero-out any residual bounce
            pc1->vel.y = 0.0f;
        }
    }
        ////  Set grounded flag if needed
        //isGrounded = true;
        //std::cout << "Grounded: Clamped to plane | newPos: " << pc1->pos << " | newVel: " << pc1->vel << std::endl;
}
