

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


// Parses the XML file and constructs the decision tree.
std::unique_ptr<DecisionTreeNode> DecisionTreeParser::parseFromFile(const std::string& filePath) {
    XMLDocument doc;

    // Load the XML file
    XMLError eResult = doc.LoadFile(filePath.c_str());
    if (eResult != XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << filePath << std::endl;
        std::cerr << "Error: " << doc.ErrorStr() << std::endl;  // Print detailed error message
        return nullptr;
    }

    // Get the root element
    const XMLElement* root = doc.RootElement();
    if (!root) {
        std::cerr << "Error: Missing root element in XML file." << std::endl;
        return nullptr;
    }

    // Check if the root element is "DecisionTree"
    if (std::string(root->Name()) != "DecisionTree") {
        std::cerr << "Error: Invalid root element. Expected 'DecisionTree', found '" << root->Name() << "'." << std::endl;
        return nullptr;
    }

    // Parse the first child element of the root
    return parseElement(root->FirstChildElement());
}

// Recursive function to parse an XML element and build a decision tree node.
std::unique_ptr<DecisionTreeNode> DecisionTreeParser::parseElement(const XMLElement* element) {
    if (!element) {
        std::cerr << "Error: Element is null while parsing XML." << std::endl;
        return nullptr;
    }

    const std::string nodeName = element->Name();

    // Handle Condition nodes
    if (nodeName == "Condition") {
        const char* conditionName = element->GetText();
        if (!conditionName) {
            std::cerr << "Error: Condition element is missing text!" << std::endl;
            return nullptr;
        }

        // Validate condition name
        if (std::string(conditionName) != "playerInRange" && std::string(conditionName) != "enemyHealthGreaterThan50") {
            std::cerr << "Error: Unknown or unsupported condition: " << conditionName << std::endl;
            return nullptr;
        }

        DecisionTreeNode* trueNode = nullptr;
        DecisionTreeNode* falseNode = nullptr;

        // Parse the True node
        const XMLElement* trueElement = element->FirstChildElement("True");
        if (trueElement) {
            trueNode = parseElement(trueElement->FirstChildElement()).release();
        }
        else {
            std::cerr << "Error: Missing 'True' branch for condition: " << conditionName << std::endl;
            return nullptr;
        }

        // Parse the False node
        const XMLElement* falseElement = element->FirstChildElement("False");
        if (falseElement) {
            falseNode = parseElement(falseElement->FirstChildElement()).release();
        }
        else {
            std::cerr << "Error: Missing 'False' branch for condition: " << conditionName << std::endl;
            return nullptr;
        }

        // Create and return an appropriate Decision node based on the condition name
        if (std::string(conditionName) == "playerInRange") {
            return std::make_unique<FloatDecision>(0.0f, 100.0f, 50.0f, trueNode, falseNode);
        }
        else if (std::string(conditionName) == "enemyHealthGreaterThan50") {
            return std::make_unique<FloatDecision>(0.0f, 100.0f, 60.0f, trueNode, falseNode);
        }

        // If condition name is unknown (additional checks are already performed), return nullptr
        std::cerr << "Error: Condition '" << conditionName << "' is not supported!" << std::endl;
        return nullptr;
    }

    // Handle Action nodes
    if (nodeName == "Action") {
        const char* actionName = element->GetText();
        if (!actionName) {
            std::cerr << "Error: Action element is missing text!" << std::endl;
            return nullptr;
        }

        // Create and return an Action node
        return std::make_unique<Action>(std::string(actionName));
    }

    // Handle unknown element names
    std::cerr << "Error: Unknown element name: " << nodeName << std::endl;
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