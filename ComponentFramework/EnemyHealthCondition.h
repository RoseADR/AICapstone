#ifndef ENEMY_HEALTH_CONDITION_H
#define ENEMY_HEALTH_CONDITION_H

#include "Condition.h"

class EnemyHealthCondition : public Condition {
    int healthThreshold;

public:
    EnemyHealthCondition(int healthThreshold_)
        : healthThreshold(healthThreshold_) {}

    bool test() override {
        // This is just an example; you can replace it with actual health checks
        return true;  // Assuming the enemy's health is greater than the threshold for simplicity
    }
};

#endif // ENEMY_HEALTH_CONDITION_H

