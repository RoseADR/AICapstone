#include "Decision.h"

Decision::Decision(DecisionTreeNode* trueNode, DecisionTreeNode* falseNode)
    : trueNode(trueNode), falseNode(falseNode) {}

DecisionTreeNode* Decision::getBranch() {
    return testValue() ? trueNode : falseNode;
}

DecisionTreeNode* Decision::makeDecision() {
    return getBranch()->makeDecision();
}