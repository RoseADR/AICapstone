#ifndef PLAYER_IN_RANGE_CONDITION_H
#define PLAYER_IN_RANGE_CONDITION_H

#include "Condition.h"

class PlayerInRangeCondition : public Condition {
    float playerDistance;
    float rangeThreshold;

public:
    PlayerInRangeCondition(float playerDistance_, float rangeThreshold_)
        : playerDistance(playerDistance_), rangeThreshold(rangeThreshold_) {}

    bool test() override {
        return playerDistance <= rangeThreshold;
    }
};

#endif // PLAYER_IN_RANGE_CONDITION_H
