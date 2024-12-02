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

//class TreeBuilder {
//
//private:
//    static DecisionTreeNode* buildNode(tinyxml2::XMLElement* element);
//public:
//    static DecisionTreeNode* buildTree(const std::string& xmlFile);
//};
//
//#endif // TREE_BUILDER_H


#include <memory>
#include <string>
#include "tinyxml2.h"
#include "DecisionTreeNode.h"
#include "Decision.h"
#include "Action.h"

using namespace tinyxml2;

// Class to handle the parsing of a decision tree from an XML file.
class DecisionTreeParser {
public:
    // Parses the XML file and constructs the decision tree.
    // @param filePath: Path to the XML file containing the decision tree definition.
    // @return A unique_ptr to the root of the decision tree, or nullptr if parsing fails.
    static std::unique_ptr<DecisionTreeNode> parseFromFile(const std::string& filePath);

private:
    // Helper function to parse an XML element and build the decision tree node.
    // @param element: XML element to parse.
    // @return A unique_ptr to the parsed DecisionTreeNode, or nullptr if parsing fails.
    static std::unique_ptr<DecisionTreeNode> parseElement(const XMLElement* element);
};



#endif // TREE_BUILDER_H