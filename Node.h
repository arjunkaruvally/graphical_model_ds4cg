#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>

class Node
{
    public:
        struct Data{
            std::string label;
            uint node_id;
        }data;
        // std::string label;
        // uint node_id;
        uint sequence_number;
        std::vector<std::string> students;
        std::vector<Node*> children;
        std::vector<Node*> parents;

    // public:
        Node(Node::Data incoming);
        Node(Node::Data incoming, int sequencep);

        // Getter functions
        std::string getLabel();
        int getSequenceNumber();
        Node* getChildAt(int position);
        std::vector<Node*> getAllChildren();
        
        //Setter functions
        bool addStudent(std::string id);
        bool addChildNode(Node* obj);
        bool addParentNode(Node* obj);
        bool memberOf(Node* obj, std::vector<Node*> vec);
};