

#include "TreeBuilder.h"
#include "tinyxml2.h"
#include "Action.h"
#include "Decision.h"
#include "FloatDecision.h"
#include <unordered_map>
#include <iostream>
#include <string>
#include "PlayerInRangeCondition.h"  
#include "EnemyHealthCondition.h"   
#include "Condition.h"  

using namespace tinyxml2;

DecisionTreeNode* TreeBuilder::buildTree(const std::string& xmlFile) {
    // Load the XML file
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(xmlFile.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << xmlFile << std::endl;
        std::cerr << "Error description: " << doc.ErrorStr() << std::endl;
        return nullptr;
    }

    // Get the root element ("DecisionTree")
    tinyxml2::XMLElement* root = doc.FirstChildElement("DecisionTree");
    if (root == nullptr) {
        std::cerr << "Error: No root element found in XML." << std::endl;
        return nullptr;
    }
    std::cout << "Root element 'DecisionTree' found, proceeding to build tree..." << std::endl;

    // Recursively build the tree from the root element
    return buildNode(root);
}

DecisionTreeNode* TreeBuilder::buildNode(tinyxml2::XMLElement* element) {
    if (element == nullptr) {
        std::cerr << "Error: Null XML element encountered." << std::endl;
        return nullptr;
    }

    std::string nodeName = element->Name();
    std::cout << "Processing node: " << nodeName << std::endl;

    if (nodeName == "Decision") {
        // Process a Decision node (such as engage)
        const char* conditionText = element->FirstChildElement("Condition") ? element->FirstChildElement("Condition")->GetText() : nullptr;

        if (conditionText == nullptr) {
            std::cerr << "Error: No 'Condition' element found for 'Decision' node." << std::endl;
            return nullptr;
        }
        std::cout << "Condition: " << conditionText << std::endl;

        // Create the appropriate condition based on the XML condition string
        Condition* condition = nullptr;

        if (conditionText == std::string("playerInRange")) {
            // For example, creating a PlayerInRangeCondition (you should pass actual data to it)
            condition = new PlayerInRangeCondition(10.0f, 15.0f);  // Example values
        }
        else if (conditionText == std::string("enemyHealthGreaterThan50")) {
            // Similarly, create an EnemyHealthCondition
            condition = new EnemyHealthCondition(50);
        }
        else {
            std::cerr << "Error: Unknown condition: " << conditionText << std::endl;
            return nullptr;
        }

        // Process the True node
        DecisionTreeNode* trueNode = buildNode(element->FirstChildElement("True"));
        if (trueNode == nullptr) {
            std::cerr << "Error: Failed to build 'True' branch of Decision node." << std::endl;
            return nullptr;
        }

        // Process the False node
        DecisionTreeNode* falseNode = buildNode(element->FirstChildElement("False"));
        if (falseNode == nullptr) {
            std::cerr << "Error: Failed to build 'False' branch of Decision node." << std::endl;
            return nullptr;
        }

        // Now create the Decision node with the condition, true and false nodes
        return new Decision(condition, trueNode, falseNode);
    }
    else if (nodeName == "Action") {
        // Process an Action node (such as seek, attack, flee, etc.)
        const char* actionName = element->GetText();
        if (actionName == nullptr) {
            std::cerr << "Error: No action name found in 'Action' node." << std::endl;
            return nullptr;
        }
        std::cout << "Action: " << actionName << std::endl;
        return new Action(actionName);
    }

    std::cerr << "Error: Unknown node type: " << nodeName << std::endl;
    return nullptr;
}



//
//
//DecisionTreeNode* parseNode(XMLElement* xmlNode, std::unordered_map<std::string, DecisionTreeNode*>& nodeMap) {
//    const char* type = xmlNode->Attribute("type");
//    const char* id = xmlNode->Attribute("id");
//
//    if (!type || !id) return nullptr;
//
//    std::string nodeType(type);
//    std::string nodeId(id);
//
//    if (nodeType == "Action") {
//        const char* actionName = xmlNode->FirstChildElement("Action")->GetText();
//        auto* action = new Action(actionName ? actionName : "Unnamed Action");
//        nodeMap[nodeId] = action;
//        return action;
//    }
//
//    if (nodeType == "Decision") {
//        float minValue = xmlNode->FirstChildElement("Condition")
//            ->FirstChildElement("Check")->FloatAttribute("minValue");
//        float maxValue = xmlNode->FirstChildElement("Condition")
//            ->FirstChildElement("Check")->FloatAttribute("maxValue");
//        const char* trueId = xmlNode->FirstChildElement("TrueNode")
//            ->FirstChildElement("Node")->Attribute("id");
//        const char* falseId = xmlNode->FirstChildElement("FalseNode")
//            ->FirstChildElement("Node")->Attribute("id");
//
//        FloatDecision* decision = new FloatDecision(minValue, maxValue, 0.0f, nullptr, nullptr);
//        nodeMap[nodeId] = decision;
//
//        decision->trueNode = nullptr; // Placeholder, will be set after all nodes are parsed
//        decision->falseNode = nullptr; // Placeholder
//
//        return decision;
//    }
//
//    return nullptr;
//}
//
//
//
//DecisionTreeNode* TreeBuilder::buildTree(const std::string& xmlFile) {
//    XMLDocument doc;
//    if (doc.LoadFile(xmlFile.c_str()) != XML_SUCCESS) {
//        std::cerr << "Failed to load XML file: " << xmlFile << std::endl;
//        return nullptr;
//    }
//
//    XMLElement* rootNode = doc.FirstChildElement("DecisionTree")->FirstChildElement("Decision");
//    if (!rootNode) {
//        std::cerr << "Invalid XML structure: No root node found." << std::endl;
//        return nullptr;
//    }
//
//    std::unordered_map<std::string, DecisionTreeNode*> nodeMap;
//
//    // Parse all nodes
//    for (XMLElement* xmlNode = rootNode; xmlNode; xmlNode = xmlNode->NextSiblingElement("Decision")) {
//        parseNode(xmlNode, nodeMap);
//    }
//
//    // Link TrueNode and FalseNode
//    for (auto& [id, node] : nodeMap) {
//        auto* decision = dynamic_cast<Decision*>(node);
//        if (decision) {
//            const char* trueId = doc.FirstChildElement("DecisionTree")->FirstChildElement("Decision")
//                ->FirstChildElement("Condition")->FirstChildElement("C")->Attribute("id");
//            const char* falseId = doc.FirstChildElement("DecisionTree")
//                ->FirstChildElement("Node")->FirstChildElement("FalseNode")
//                ->FirstChildElement("Node")->Attribute("id");
//
//            decision->trueNode = nodeMap[trueId];
//            decision->falseNode = nodeMap[falseId];
//        }
//    }
//
//    return nodeMap["root"];
//}