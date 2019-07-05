#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

#include "Node.h"
#include "Graph.h"

// Constructor
Graph::Graph(Node* rootp, Node* endp){
    root = rootp;
    end = endp;
}

// Find value in an array of values
bool Graph::valueIn(Node* val, std::vector<Node*> array){
    return std::find(array.begin(), array.end(), val) != array.end();
}

// Think if this is required
void Graph::bfsParse(){
    // BFS show output the graph

    std::vector<Node*> visited;
    std::vector<Node*> queue;
    Node* s=NULL;

    // Mark current node as visited and queue it

    BOOST_LOG_TRIVIAL(trace)<<"Pushing to queues...";
    visited.push_back(root);
    queue.push_back(root);

    BOOST_LOG_TRIVIAL(trace)<<"BFS Parse Started...";

    while(queue.size()>0){
        s = queue[0];
        BOOST_LOG_TRIVIAL(info)<<s->getLabel();
        queue.erase(queue.begin());

        for(Node* child : s->getAllChildren()){
            if(!this->valueIn(child, visited)){
                visited.push_back(child);
                queue.push_back(child);
            }
        }
    }

    BOOST_LOG_TRIVIAL(trace)<<"BFS Parse Done";
}


Node* Graph::addNode(std::string student_id, Node* current_node, std::string label, int sequence_number){
    Node* node=NULL;
    BOOST_LOG_TRIVIAL(trace)<<"Step 1: Searching for existing nodes";
    ///////// Step 1: Search for the node
    std::vector<Node*> node_children=current_node->getAllChildren();

    for(uint i=0; i<node_children.size(); i++){
        Node* child=node_children[i];
        BOOST_LOG_TRIVIAL(trace)<<"Comparing "<<child->getLabel()<<" with "<<label;
        if(child->getLabel().compare(label)==0){
            BOOST_LOG_TRIVIAL(trace)<<"Match Found";
            node=child;
            break;
        }
    }

    BOOST_LOG_TRIVIAL(trace)<<"Step 2: Creating and adding node if not found";
    ///////// Step 2: create and add node to graph if not found
    if(node==NULL){
        BOOST_LOG_TRIVIAL(trace)<<"no node from step 1, creating new node...";
        node = new Node(label, sequence_number);
    }
    BOOST_LOG_TRIVIAL(trace)<<"adding student to node...";
    // Add student to node
    node->addStudent(student_id);
    

    BOOST_LOG_TRIVIAL(trace)<<"Step 3: Making changes to graph...";;
    //////// Step 3: do necessary modifications to graph
    current_node->addChildNode(node);
    

    BOOST_LOG_TRIVIAL(trace)<<"Graph::addNode() done";
    // return pointer to node
    return node;
}


void Graph::wrap_up(std::string student_id, Node* current_node){
    current_node->addChildNode(end);
    end->addStudent(student_id);
}