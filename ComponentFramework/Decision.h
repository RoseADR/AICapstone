#ifndef DECISION_H
#define DECISION_H

#include "DecisionTreeNode.h"


class Decision : public DecisionTreeNode {
    
public:
    DecisionTreeNode* trueNode;
    DecisionTreeNode* falseNode;
    Decision(DecisionTreeNode* trueNode, DecisionTreeNode* falseNode);
    virtual bool testValue() const = 0;
    DecisionTreeNode* getBranch();
    DecisionTreeNode* makeDecision() override;
};

#endif // DECISION_H
