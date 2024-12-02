#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include "DecisionTreeNode.h"
#include "Action.h"
#include "Decision.h"
// #include "MakeDecision.h"
#include "FloatDecision.h"
#include <string>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"

class TreeBuilder {

private:
    static DecisionTreeNode* buildNode(tinyxml2::XMLElement* element);
public:
    static DecisionTreeNode* buildTree(const std::string& xmlFile);
};

#endif // TREE_BUILDER_H