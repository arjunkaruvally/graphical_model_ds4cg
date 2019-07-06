#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

#include "utils.h"
#include "Node.h"


Node::Node(Node::Data incoming){
    data = incoming;
    sequence_number = 0;
    BOOST_LOG_TRIVIAL(trace)<<"Node "<<data.label<<" created";
}


Node::Node(Node::Data incoming, int sequencep){
    data = incoming;
    sequence_number = sequencep;
}


std::string Node::getLabel(){
    return data.label;
}


int Node::getSequenceNumber(){
    return sequence_number;
}


bool Node::addStudent(std::string id){
    BOOST_LOG_TRIVIAL(trace)<<"Pushing student id: "<<id<<"...";
    this->students.push_back(id);
    
    return true;
}

bool Node::memberOf(Node* obj, std::vector<Node*> vec){
    BOOST_LOG_TRIVIAL(trace)<<"Calculating memberOf";
    for(Node* member : vec){
        BOOST_LOG_TRIVIAL(trace)<<"comparing "<<member->data.label<<" and "<<obj->data.label;
        if(member->data.node_id==obj->data.node_id){
            return true;
        }
    }
    return false;
}

bool Node::addChildNode(Node* obj){
    BOOST_LOG_TRIVIAL(trace)<<"Adding child node: "<<obj->getLabel()<<" in parent: "<<data.label<<"...";
    if(!memberOf(obj, children)){
        BOOST_LOG_TRIVIAL(trace)<<"Child Added";
        children.push_back(obj);
        return true;
    }
    BOOST_LOG_TRIVIAL(trace)<<"Duplicate detected";
    return false;
}


bool Node::addParentNode(Node* obj){
    BOOST_LOG_TRIVIAL(trace)<<"Adding parent node: "<<obj->getLabel()<<" in child: "<<data.label<<"...";
    parents.push_back(obj);
    
    return true;
}


Node* Node::getChildAt(int position){
    return children[position];
}


std::vector<Node*> Node::getAllChildren(){
    return children;
}