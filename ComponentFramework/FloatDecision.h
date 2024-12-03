//#ifndef FLOAT_DECISION_H
//#define FLOAT_DECISION_H
//
//#include "Decision.h"
//
//class FloatDecision : public Decision {
//    float minValue;
//    float maxValue;
//    float testValueValue; // This holds the value to be checked.
//
//public:
//    FloatDecision(float min, float max, float value, DecisionTreeNode* trueNode, DecisionTreeNode* falseNode)
//        : Decision(/*condition,*/ trueNode, falseNode), minValue(min), maxValue(max), testValueValue(value) {}
//
//    bool testValue()  {
//        return testValueValue >= minValue && testValueValue <= maxValue;
//    }
//};
//
//#endif // FLOAT_DECISION_H


#ifndef FLOAT_DECISION_H
#define FLOAT_DECISION_H

#include "Decision.h"
#include "Actor.h"

class FloatDecision : public Decision {
    float minValue;
    float maxValue;
    std::string type;  // Decision type: "PlayerHealth", "PlayerFacingEnemy", "PlayerDistance"
    Actor* actor;      // Enemy actor
    Actor* targetActor; // Player actor

public:
    FloatDecision(const std::string& type, float min, float max,
        Actor* actor, Actor* targetActor,
        DecisionTreeNode* trueNode, DecisionTreeNode* falseNode);

    bool testValue() const override;
};

#endif // FLOAT_DECISION_H
