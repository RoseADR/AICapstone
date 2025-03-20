#include "ScottCollisionSystem.h"
#include <PMath.h>
#include <MMath.h>

using namespace MATH;

void CollisionSystem::Update(const float deltaTime) {

    for (size_t i = 0; i < collidingActors.size()-1; ++i) {
        Ref<PhysicsComponent> pc1 = collidingActors[i]->GetComponent<PhysicsComponent>();
        Ref<TransformComponent> tc1 = collidingActors[i]->GetComponent<TransformComponent>();
        Ref<CollisionComponent> cc1 = collidingActors[i]->GetComponent<CollisionComponent>();

        if (!cc1 || (!pc1 && !tc1)) continue;
         
        Sphere s1;
        s1.r = cc1->GetRadius();
        s1.center = pc1 ? pc1->pos : tc1->GetPosition();

        for (size_t j = i + 1; j < collidingActors.size(); ++j) {
            Ref<PhysicsComponent> pc2 = collidingActors[j]->GetComponent<PhysicsComponent>();
            Ref<TransformComponent> tc2 = collidingActors[j]->GetComponent<TransformComponent>();
            Ref<CollisionComponent> cc2 = collidingActors[j]->GetComponent<CollisionComponent>();
            if (!cc2 || (!pc2 && !tc2)) continue;

            // Factory Collision Bounds
            Vec3 factoryPos = factory->GetComponent<TransformComponent>()->GetPosition();
            float factoryX = factoryPos.x;
            float factoryThreshold = 30.0f;  // Define a threshold for factory

            Vec3 obj1Pos = tc1 ? tc1->GetPosition() : pc1->pos;
            Vec3 obj2Pos = tc2 ? tc2->GetPosition() : pc2->pos;
         // Bridge Collision Bounds
            Vec3 bridgePos = bridge->GetComponent<TransformComponent>()->GetPosition();
            float bridgeX = bridgePos.x;
            float bridgeThreshold = 30.0f;  // Adjust threshold for bridge

            if (obj1Pos.x < factoryX - factoryThreshold || obj1Pos.x > factoryX + factoryThreshold ||
                obj2Pos.x < factoryX - factoryThreshold || obj2Pos.x > factoryX + factoryThreshold /*&&  
                obj1Pos.x < bridgeX - bridgeThreshold || obj1Pos.x > bridgeX + bridgeThreshold ||
                obj2Pos.x < bridgeX - bridgeThreshold || obj2Pos.x > bridgeX + bridgeThreshold*/) {
                continue;  // Skip collision check if objects are out of factory bounds
            }

            if (cc2->GetColliderType() == ColliderType::Sphere) {
                Sphere s2;
                s2.r = cc2->GetRadius();
                s2.center = pc2 ? pc2->pos : tc2->GetPosition();

                if (SphereSphereCollisionDetection(s1, s2)) {
                    SphereSphereCollisionResponse(s1, pc1, s2, pc2);
                }
            }
            else if (cc2->GetColliderType() == ColliderType::PLANE) {
                Plane p1;
                p1.n = cc2->normal;
                p1.d = VMath::dot(p1.n, tc2->GetPosition());

                if (SpherePlaneCollisionDetection(s1, p1)) {
                    SpherePlaneCollisionResponse(s1, pc1, p1);
                }
            }

            std::cout << "Collision detected between " << i << " and " << j << std::endl;
        }
    }
}

  /*  for (size_t i = 0; i < collidingActors.size(); ++i) {
        Ref<PhysicsComponent> pc1 = collidingActors[i]->GetComponent<PhysicsComponent>();
        Ref<TransformComponent> tc1 = collidingActors[i]->GetComponent<TransformComponent>();
        Ref<CollisionComponent> cc1 = collidingActors[i]->GetComponent<CollisionComponent>();

        if (!cc1 || (!pc1 && !tc1));

        Sphere s1;
        s1.r = cc1->GetRadius();
        s1.center = pc1 ? pc1->pos : tc1->GetPosition();

        for (size_t j = i + 1; j < collidingActors.size(); ++j) {
            Ref<PhysicsComponent> pc2 = collidingActors[j]->GetComponent<PhysicsComponent>();
            Ref<TransformComponent> tc2 = collidingActors[j]->GetComponent<TransformComponent>();
            Ref<CollisionComponent> cc2 = collidingActors[j]->GetComponent<CollisionComponent>();

            if (!cc2 || (!pc2 && !tc2));

            if (cc2->GetColliderType() == ColliderType::Sphere) {
                Sphere s2;
                s2.r = cc2->GetRadius();
                s2.center = pc2 ? pc2->pos : tc2->GetPosition();

                if (SphereSphereCollisionDetection(s1, s2)) {
                    SphereSphereCollisionResponse(s1, pc1, s2, pc2);
                }
            }
            else if (cc2->GetColliderType() == ColliderType::PLANE) {
                Plane p1;
                p1.n = cc2->normal;
                p1.d = VMath::dot(p1.n, tc2->GetPosition());

                if (SpherePlaneCollisionDetection(s1, p1)) {
                    SpherePlaneCollisionResponse(s1, pc1, p1);
                }
            }

            std::cout << "Collision detected between " << i << " and " << j << std::endl;
        }
    }

}*/

//    for (size_t i = 0; i < collidingActors.size(); ++i) {
//        for (size_t j = i + 1; j < collidingActors.size(); ++j) {
//
//            Sphere s1, s2;  /// I'm just going to do sphere-sphere collions first
//            s1.r = collidingActors[i]->GetComponent<CollisionComponent>()->radius;
//            //s1.center = collidingActors[i]->GetComponent<PhysicsComponent>()->pos;
//            Ref<PhysicsComponent> spherePC = collidingActors[i]->GetComponent<PhysicsComponent>();
//            Ref<TransformComponent> sphereTC = collidingActors[i]->GetComponent<TransformComponent>();
//
//            //// Use PhysicsComponent if available, otherwise fallback to TransformComponent
//            if (spherePC) {
//                s1.center = spherePC->pos;
//            }
//            else if (sphereTC) {
//                s1.center = sphereTC->GetPosition();
//            }
//            //else {
//            //    Debug::Error("Sphere object has no valid position source", __FILE__, __LINE__);
//            //    
//            //}
//
//            std::string hop2 = "s1.center: (" +
//                std::to_string(s1.center.x) + ", " +
//                std::to_string(s1.center.y) + ", " +
//                std::to_string(s1.center.z) + ")";
//
//
//            if (collidingActors[j]->GetComponent<CollisionComponent>()->colliderType == ColliderType::Sphere) {
//
//                s2.r = collidingActors[j]->GetComponent<CollisionComponent>()->radius;
//                s2.center = collidingActors[j]->GetComponent<PhysicsComponent>()->pos;
//               
//                if (SphereSphereCollisionDetection(s1, s2) == true) {
//                    Ref<PhysicsComponent> pc1 = collidingActors[i]->GetComponent<PhysicsComponent>();
//                    Ref<PhysicsComponent> pc2 = collidingActors[j]->GetComponent<PhysicsComponent>();
//                    SphereSphereCollisionResponse(s1, pc1, s2, pc2);
//
//                    std::cout << "SphereSphere Collision" + hop2 << std::endl;
//                }
//            }
//
//            if (collidingActors[j]->GetComponent<CollisionComponent>()->colliderType == ColliderType::PLANE) {
//
//                Plane p1;
//                p1.n = collidingActors[j]->GetComponent<CollisionComponent>()->normal;
//               // p1.d = VMath::dot(p1.n, (collidingActors[j]->GetComponent<TransformComponent>()->GetPosition()));
//
//                Ref<TransformComponent> planeTC = collidingActors[j]->GetComponent<TransformComponent>();
//
//                if (planeTC) {
//                    p1.d = VMath::dot(p1.n, planeTC->GetPosition());
//                }
//                else {
//                    Debug::Error("Plane object has no TransformComponent", __FILE__, __LINE__);
//                   
//                }
//
//              std::string hop = "p1.n: (" +
//                  std::to_string(p1.n.x) + ", " +
//                  std::to_string(p1.n.y) + ", " +
//                  std::to_string(p1.n.z) + ")";
//
//                if (SpherePlaneCollisionDetection(s1, p1) == true) {
//                    Ref<PhysicsComponent> pc1 = collidingActors[i]->GetComponent<PhysicsComponent>();
//                    //Ref<PhysicsComponent> pc2 = collidingActors[j]->GetComponent<PhysicsComponent>();
//                    SpherePlaneCollisionResponse(s1, pc1, p1);
//                   
//                    std::cout << "SpherePlane Collision" + hop << std::endl;
//                }
//            }
//        }
//    }
//
//}


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
    float dist = VMath::dot(s1.center, p1.n) - p1.d;

    if (fabs(dist <= s1.r)) {
        //  std::cout << "SpherePlane Collision" << std::endl;
        return true;
    }
    return false;

}

void CollisionSystem::SpherePlaneCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Plane p2/*, Ref<PhysicsComponent> tc2*/)
{
    float dist = VMath::dot(s1.center, p2.n) - p2.d;

    // If the sphere is penetrating the plane, apply MTV
    if (dist < s1.r) {
        float penetrationDepth = s1.r - dist;  // Overlap amount

        // Move the sphere completely out of the plane
        pc1->pos += p2.n * penetrationDepth;

        // Stop downward movement
        float velocityAlongNormal = VMath::dot(pc1->vel, p2.n);
        if (velocityAlongNormal < 0.0f) {
            pc1->vel -= p2.n * velocityAlongNormal;  // Remove movement in the plane normal direction
        }
    }

}
