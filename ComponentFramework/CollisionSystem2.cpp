//#include "CollisionSystem2.h"
//#include "TransformComponent.h"
//#include <iostream>
//#include "CollisionComponent2.h"
//
//bool CollisionHandler::CheckCollision(const std::shared_ptr<Actor>& actor1, const std::shared_ptr<Actor>& actor2) {
//    auto collider1 = actor1->GetComponent<CollisionComponent>();
//    auto collider2 = actor2->GetComponent<CollisionComponent>();
//
//    if (!collider1 || !collider2) return false;
//
//    Vec3 min1 = collider1->GetMinBounds();
//    Vec3 max1 = collider1->GetMaxBounds();
//    Vec3 min2 = collider2->GetMinBounds();
//    Vec3 max2 = collider2->GetMaxBounds();
//
//    //// AABB overlap check
//    //return (min1.x <= max2.x && max1.x >= min2.x) &&
//    //    (min1.y <= max2.y && max1.y >= min2.y) &&
//    //    (min1.z <= max2.z && max1.z >= min2.z);
//
//
//   /* std::cout << "[CollisionHandler] Checking Collision between: "
//        << actor1.get() << " and " << actor2.get() << "\n";
//    std::cout << "  Object 1 Bounds: Min (" << min1.x << ", " << min1.y << ", " << min1.z
//        << ") - Max (" << max1.x << ", " << max1.y << ", " << max1.z << ")\n";
//    std::cout << "  Object 2 Bounds: Min (" << min2.x << ", " << min2.y << ", " << min2.z
//        << ") - Max (" << max2.x << ", " << max2.y << ", " << max2.z << ")\n";*/
//
//    return (min1.x <= max2.x && max1.x >= min2.x) &&
//        (min1.y <= max2.y && max1.y >= min2.y) &&
//        (min1.z <= max2.z && max1.z >= min2.z);
//}
//
//void CollisionHandler::ResolveCollision(const std::shared_ptr<Actor>& actor1, const std::shared_ptr<Actor>& actor2) {
//    // Get collision components
//    auto collider1 = actor1->GetComponent<CollisionComponent>();
//    auto collider2 = actor2->GetComponent<CollisionComponent>();
//
//    if (!collider1 || !collider2) return;
//
//    Vec3 min1 = collider1->GetMinBounds();
//    Vec3 max1 = collider1->GetMaxBounds();
//
//    Vec3 min2 = collider2->GetMinBounds();
//    Vec3 max2 = collider2->GetMaxBounds();
//
//    // Only adjust actor1 (character) to sit above actor2 (road)
//   
//}
//
//
//
//
