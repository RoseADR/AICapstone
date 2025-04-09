#include "ScottCollisionSystem.h"
#include <PMath.h>
#include <MMath.h>
#include <SDL.h>


using namespace MATH;
using namespace std;



void CollisionSystem::Update(const float deltaTime) {

    std::unordered_set<std::string> updatedCollisions;

    Ref<PhysicsComponent> pc1 = character->GetComponent<PhysicsComponent>();
    Ref<TransformComponent> tc1 = character->GetComponent<TransformComponent>();
    Ref<CollisionComponent> cc1 = character->GetComponent<CollisionComponent>();
    if (!cc1 || (!pc1 && !tc1)) return;

    Sphere s1;
    s1.r = cc1->GetRadius();
    s1.center = pc1 ? pc1->pos : tc1->GetPosition();

    Vec3 obj1Pos = pc1 ? pc1->pos : tc1->GetPosition();

    for (size_t i = 0; i < collidingActors.size(); ++i) {
        Ref<PhysicsComponent> pc2 = collidingActors[i]->GetComponent<PhysicsComponent>();
        Ref<TransformComponent> tc2 = collidingActors[i]->GetComponent<TransformComponent>();
        Ref<CollisionComponent> cc2 = collidingActors[i]->GetComponent<CollisionComponent>();

        Actor* parentActor = static_cast<Actor*>(cc2->parent);

        if (!cc2 || (!pc2 && !tc2)) continue;

        Vec3 obj2Pos = pc2 ? pc2->pos : tc2->GetPosition();

        if (cc2->GetColliderType() == ColliderType::Sphere) {
            Sphere s2;
            s2.r = cc2->radius;
            s2.center = pc2 ? pc2->pos : tc2->GetPosition();

            if (SphereSphereCollisionDetection(s1, s2)) {
             
                SphereSphereCollisionResponse(s1, pc1, s2, pc2);
                if (parentActor) {
                    std::string collisionKey = character->GetName() + "-" + parentActor->GetName();
                    if (currentCollisions.find(collisionKey) == currentCollisions.end()) {
                        std::cout << " SPHERE Collision with actor " << i
                            << ": " << parentActor->GetName() << std::endl;
                    }
                    updatedCollisions.insert(collisionKey); // Always add it so we know it's still active
                }
            }
        }
        else if (cc2->GetColliderType() == ColliderType::PLANE) {
            Plane p;
            p.n = cc2->normal;
            p.d = cc2->dist; 
        
            if (SpherePlaneCollisionDetection(s1, p)) {
               
                SpherePlaneCollisionResponse(s1, pc1, p);
                if (parentActor) {
                    std::string collisionKey = character->GetName() + "-" + parentActor->GetName();
                    if (currentCollisions.find(collisionKey) == currentCollisions.end()) {
                        std::cout << " PLANE Collision with actor " << i
                            << ": " << parentActor->GetName() << std::endl;
                    }
                    updatedCollisions.insert(collisionKey); // Always add it so we know it's still active
                }
            }
        }
        else if (cc2->GetColliderType() == ColliderType::AABB) {

            AABB bb1 = cc2->GetAABB();
            bb1.center = tc2->GetPosition();
           // worldAABB.center += worldPos; // move AABB to world space
          
           /* AABB bb1 = cc2->GetAABB();
            bb1.center = tc2->GetPosition() + bb1.center;
            worldAABB.center += worldPos;*/
                           
            
            if (SphereAABBCollisionDetection(s1, bb1)) {
                SphereAABBCollisionResponse(s1, pc1, bb1);
                if (parentActor) {
                    std::string collisionKey = character->GetName() + "-" + parentActor->GetName();
                    if (currentCollisions.find(collisionKey) == currentCollisions.end()) {
                        std::cout << " SPHERE Collision with actor " << i
                            << ": " << parentActor->GetName() << std::endl;
                    }
                    updatedCollisions.insert(collisionKey); // Always add it so we know it's still active
                }
               
            }
        }
    }

    currentCollisions = std::move(updatedCollisions);
  
    if (isGrounded) {
        pc1->accel.y = 0.0f;  // No gravity when grounded
        pc1->vel.y = 0.0f;  // Optional: keep vertical velocity zero
    }
    else {
        //pc1->accel.y = -9.81f;  // Restore gravity when not grounded
    }
    isGrounded = false; // Reset for next frame

}

bool CollisionSystem::SphereSphereCollisionDetection(const Sphere& s1, const Sphere& s2) const {
    float distance = VMath::distance(s1.center, s2.center);
    if (distance < s1.r + s2.r) {
        //std::cout << "SphereSphere Collision" << std::endl;
        return true;
    }
    return false;
}


//bool CollisionSystem::AABBAABBCollisionDetection(const AABB & bb1, const AABB & bb2) const {
//   if (fabs(bb1.center.x - bb2.center.x) <= (bb1.rx + bb2.rx)) return false;
//   if (fabs(bb1.center.y - bb2.center.y) <= (bb1.ry + bb2.ry)) return false;
//   if (fabs(bb1.center.z - bb2.center.z) <= (bb1.rz + bb2.rz)) return false;
//   return true;
//}


bool CollisionSystem::SphereAABBCollisionDetection(const Sphere& s, const AABB& box) const {
    float x = std::max(box.center.x - box.rx, std::min(s.center.x, box.center.x + box.rx));
    float y = std::max(box.center.y - box.ry, std::min(s.center.y, box.center.y + box.ry));
    float z = std::max(box.center.z - box.rz, std::min(s.center.z, box.center.z + box.rz));
    Vec3 closestPoint(x, y, z);

    // Compute vector from closest point to sphere center
    Vec3 d = s.center - closestPoint;
    float distSquared = d.x * d.x + d.y * d.y + d.z * d.z;

    // DEBUG OUTPUT
   /* std::cout << "-------- Sphere vs AABB Debug --------" << std::endl;
    std::cout << "Sphere Center: " << s.center << " Radius: " << s.r << std::endl;
    std::cout << "AABB Center: " << box.center
        << " Half Extents: (" << box.rx << ", " << box.ry << ", " << box.rz << ")" << std::endl;
    std::cout << "Closest Point on AABB: " << closestPoint << std::endl;
    std::cout << "Distance Squared: " << distSquared
        << " vs Radius^2: " << s.r * s.r << std::endl;
    std::cout << "Colliding? " << (distSquared < (s.r * s.r) ? "YES" : "NO") << std::endl;
    std::cout << "--------------------------------------" << std::endl;*/

    if (distSquared <= (s.r * s.r)) {
        return true;
    }
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

//void CollisionSystem::AABBAABBCollisionResponse(AABB bb1, Ref<PhysicsComponent> pc1, AABB bb2, Ref<PhysicsComponent> pc2) {
//    Vec3 diff = bb1.center - bb2.center;
//    Vec3 overlap;
//
//    overlap.x = (bb1.rx + bb2.rx) - fabs(diff.x);
//    overlap.y = (bb1.ry + bb2.ry) - fabs(diff.y);
//    overlap.z = (bb1.rz + bb2.rz) - fabs(diff.z);
//
//    if (overlap.x < overlap.y && overlap.x < overlap.z) {
//        pc1->pos.x += (diff.x > 0 ? 1 : -1) * overlap.x;
//        pc1->vel.x = 0;
//    }
//    else if (overlap.y < overlap.z) {
//        pc1->pos.y += (diff.y > 0 ? 1 : -1) * overlap.y;
//        pc1->vel.y = 0;
//    }
//    else {
//        pc1->pos.z += (diff.z > 0 ? 1 : -1) * overlap.z;
//        pc1->vel.z = 0;
//    }
//}


void CollisionSystem::SphereAABBCollisionResponse(Sphere s, Ref<PhysicsComponent> pc, AABB box) {
    // find closest point on AABB
    Vec3 closestPoint;
    closestPoint.x = std::max(box.center.x - box.rx, std::min(s.center.x, box.center.x + box.rx));
    closestPoint.y = std::max(box.center.y - box.ry, std::min(s.center.y, box.center.y + box.ry));
    closestPoint.z = std::max(box.center.z - box.rz, std::min(s.center.z, box.center.z + box.rz));

    // calculate penetration
    Vec3 penetrationVec = s.center - closestPoint;
    float penetrationVecMagSq = VMath::dot(penetrationVec, penetrationVec);
    if (penetrationVecMagSq < VERY_SMALL) return; // avoid zero-length normal

    float penetrationVecMag = sqrt(penetrationVecMagSq);
    float penetrationDepth = s.r - penetrationVecMag;

    if (penetrationDepth <= VERY_SMALL) return; // small threshold for stability

    Vec3 n = penetrationVec / penetrationVecMag;

    // MTV resolution - correct actor position
    pc->pos += n * penetrationDepth;

    // Zero out the velocity along the collision normal completely
    float velocityIntoBox = VMath::dot(pc->vel, n);
    if (velocityIntoBox < 0.0f) {
        pc->vel -= n * velocityIntoBox;
    }

    // Grounding logic
    if (fabs(n.y) > 0.5f) {
        pc->vel.y = 0.0f;
        isGrounded = true;
    }

   // std::cout << "Collision resolved: MTV = " << (n * penetrationDepth) << ", New Pos = " << pc->pos << "\n";
}


bool CollisionSystem::SpherePlaneCollisionDetection(const Sphere& s1, const Plane& p1)
{
    float dist = VMath::dot(s1.center, p1.n) - p1.d;

    if (fabs(dist <= s1.r)) {
        //  std::cout << "SpherePlane Collision" << std::endl;
        return true;
    }
  
}

void CollisionSystem::SpherePlaneCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Plane p2)
{
    float dist = VMath::dot(s1.center, p2.n) - p2.d;

    if (dist < s1.r) {
        float penetrationDepth = s1.r - dist;

        //  stop velocity into the plane
        float velocityAlongNormal = VMath::dot(pc1->vel, p2.n);
        if (velocityAlongNormal < 0.0f) {
            pc1->vel -= p2.n * velocityAlongNormal;
            pc1->pos += p2.n * penetrationDepth;
            //  zero-out any residual bounce
            pc1->vel.y = 0.0f;
        }
    }
        //  Set grounded flag if needed
        isGrounded = true;
     
}
