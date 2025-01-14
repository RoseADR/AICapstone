#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <memory>
#include "Actor.h"

class CollisionHandler {
public:
    static bool CheckCollision(const std::shared_ptr<Actor>& actor1, const std::shared_ptr<Actor>& actor2);
};

#endif // COLLISIONSYSTEM_H
