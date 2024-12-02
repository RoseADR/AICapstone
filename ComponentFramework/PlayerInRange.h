//#ifndef PLAYER_IN_RANGE_H
//#define PLAYER_IN_RANGE_H
//#include "Decision.h"
//#include "VMath.h"
//#include "AiComponent.h"
//
//
//class PlayerInRange : public Decision
//
//{
//public:
//    PlayerInRange() : Decision()
//    {}
//
//    PlayerInRange(
//        DecisionTreeNode* trueNode_,
//        DecisionTreeNode* falseNode_,
//        AiComponent* owner_
//    ) :
//        Decision(trueNode_, falseNode_, owner_)
//    {}
//
//    bool testValue();
//};
//#endif

#ifndef PLAYER_IN_RANGE_H
#define PLAYER_IN_RANGE_H
#include "Decision.h"
#include "VMath.h"
//#include "AiComponent.h"

class PlayerInRange : public Decision

{
public:
    PlayerInRange() : Decision()
    {}

    PlayerInRange(
        DecisionTreeNode* trueNode_,
        DecisionTreeNode* falseNode_
    ) :
        Decision(trueNode_, falseNode_)
    {}

    bool testValue();
};
#endif