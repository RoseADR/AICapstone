#ifndef FLOAT_DECISION_H
#define FLOAT_DECISION_H

#include "Decision.h"

class FloatDecision : public Decision {
    float minValue;
    float maxValue;
    float testValueValue; // This holds the value to be checked.

public:
    FloatDecision(float min, float max, float value, DecisionTreeNode* trueNode, DecisionTreeNode* falseNode)
        : Decision(condition, trueNode, falseNode), minValue(min), maxValue(max), testValueValue(value) {}

    bool testValue()  {
        return testValueValue >= minValue && testValueValue <= maxValue;
    }
};

#endif // FLOAT_DECISION_H
