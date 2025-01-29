//#include "ScottCollisionSystem.h"
//#include "ScottCollisionComponent.h"
//#include "PhysicsComponent.h"
//
//
//
//void CollisionSystem::Update(const float deltaTime) {
//    for (size_t i = 0; i < collidingActors.size(); ++i) {
//        for (size_t j = i + 1; j < collidingActors.size(); ++j) {
//            Sphere s1, s2;  // Sphere-sphere collision first
//            auto comp1 = collidingActors[i]->GetComponent<PhysicsComponent>();
//            auto comp2 = collidingActors[j]->GetComponent<PhysicsComponent>();
//
//            if (!comp1 || !comp2) continue;
//
//            s1.r = collidingActors[i]->GetComponent<SFCollisionComponent>()->radius;
//            s1.center = comp1->GetPosition(); // Ensure GetPosition exists in TransformComponent
//
//            s2.r = collidingActors[j]->GetComponent<SFCollisionComponent>()->radius;
//            s2.center = comp2->GetPosition();
//
//            if (SphereSphereCollisionDetection(s1, s2)) {
//                Ref<PhysicsComponent> pc1 = comp1;
//                Ref<PhysicsComponent> pc2 = comp2;
//                SphereSphereCollisionResponse(s1, pc1, s2, pc2);
//            }
//        }
//    }
//}
//
//bool CollisionSystem::SphereSphereCollisionDetection(const Sphere& s1, const Sphere& s2) const {
//    float distance = VMath::distance(s1.center, s2.center);
//    return distance < s1.r + s2.r;
//}
//
//void CollisionSystem::SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2) {
//    float e = 1.0f; // Coefficient of restitution
//    Vec3 L = s1.center - s2.center;
//    Vec3 n = VMath::normalize(L);
//
//    Vec3 v1 = pc1->getVel();
//    Vec3 v2 = pc2->getVel();
//    float m1 = pc1->getMass();
//    float m2 = pc2->getMass();
//
//    float v1p = VMath::dot(v1, n);
//    float v2p = VMath::dot(v2, n);
//
//    if (v1p - v2p > 0.0f) {
//        return; // Objects are separating, no collision response
//    }
//
//    float v1pnew = ((m1 - e * m2) * v1p + (1.0f + e) * m2 * v2p) / (m1 + m2);
//    float v2pnew = ((m2 - e * m1) * v2p + (1.0f + e) * m1 * v1p) / (m1 + m2);
//
//    pc1->SetVelocity(v1 + (v1pnew - v1p) * n);
//    pc2->SetVelocity(v2 + (v2pnew - v2p) * n);
//}