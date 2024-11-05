#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <iostream>
#include "Node.h"

class Graph
{
private:
    // A list of nodes 
    std::map<int, Node*> node;     // essentially an array, node[2]

    // A 2D matrix of the connection costs/weights
    std::vector< std::vector< float > > cost;
    // cost [1][3] NOT cost[1,3]

public:
    Graph();
    ~Graph();
    bool OnCreate(std::vector< Node* > nodes_);
    int numNodes();
    Node* getNode(int label) { return node[label]; }
    void addWeightedConnection(Node* fromNode, Node* toNode, float weight);
    std::vector<Node*> neighbours(Node* fromNode);
    std::vector<Node*> findPath(Node* startNode, Node* goalNode);
};

#endif