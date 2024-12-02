//#ifndef DECISION_H
//#define DECISION_H
//
//#include "DecisionTreeNode.h"
//
//
//class Decision : public DecisionTreeNode {
//    
//public:
//    DecisionTreeNode* trueNode;
//    DecisionTreeNode* falseNode;
//    Decision(DecisionTreeNode* trueNode, DecisionTreeNode* falseNode);
//    virtual bool testValue() const = 0;
//    DecisionTreeNode* getBranch();
//    DecisionTreeNode* makeDecision() override;
//};
//
//#endif // DECISION_H

//#ifndef DECISION_H
//#define DECISION_H
//#include "DecisionTreeNode.h"
//#include "TransformComponent.h"
//
//class Decision :
//    public DecisionTreeNode
//{
//public:
//    DecisionTreeNode* trueNode;
//    DecisionTreeNode* falseNode;
//   // Ref<TransformComponent> owner;
//
//    virtual DecisionTreeNode* getBranch();
//
//public:
//    Decision() : trueNode{ nullptr }, falseNode{ nullptr }
//    {}
//    Decision(DecisionTreeNode* trueNode_, DecisionTreeNode* falseNode_) :
//        trueNode{ trueNode_ },
//        falseNode{ falseNode_ }
//    {}
//
//    virtual bool testValue() = 0;
//    virtual DecisionTreeNode* makeDecision();
//};
//
//#endif

#ifndef DECISION_H
#define DECISION_H

#include "DecisionTreeNode.h"
#include "Condition.h"

class Decision : public DecisionTreeNode {

public:
    Condition* condition;  // Condition for this decision
    DecisionTreeNode* trueNode;
    DecisionTreeNode* falseNode;

    Decision(Condition* condition_, DecisionTreeNode* trueNode_, DecisionTreeNode* falseNode_)
        : condition(condition_), trueNode(trueNode_), falseNode(falseNode_) {}

    DecisionTreeNode* makeDecision() override {
        if (condition->test()) {
            return trueNode;
        }
        return falseNode;
    }
};

#endif // DECISION_H
