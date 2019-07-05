#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "Node.h"

// Definition
class Graph
{
    public:
        Node* root;
        Node* end;

    // public:
        Graph(Node* rootp, Node* endp);

        Node* addNode(std::string student_id, Node* current_node, std::string label, int sequence_number);
        void wrap_up(std::string student_id, Node* current_node);
        
        void bfsParse();
        bool valueIn(Node* val, std::vector<Node*> array);
};