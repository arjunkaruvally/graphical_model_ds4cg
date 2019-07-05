#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

#include "Node.h"


Node::Node(std::string name){
    label = name;
    sequence_number = 0;
    BOOST_LOG_TRIVIAL(trace)<<"Node "<<label<<" created";
}


Node::Node(std::string name, int sequencep){
    label = name;
    sequence_number = sequencep;
}


std::string Node::getLabel(){
    return label;
}


int Node::getSequenceNumber(){
    return sequence_number;
}


bool Node::addStudent(std::string id){
    BOOST_LOG_TRIVIAL(trace)<<"Pushing student id: "<<id<<"...";
    this->students.push_back(id);
    
    return true;
}


bool Node::addChildNode(Node* obj){
    BOOST_LOG_TRIVIAL(trace)<<"Adding child node: "<<obj->getLabel()<<" in parent: "<<label<<"...";
    children.push_back(obj);
    
    return true;
}


bool Node::addParentNode(Node* obj){
    BOOST_LOG_TRIVIAL(trace)<<"Adding parent node: "<<obj->getLabel()<<" in child: "<<label<<"...";
    parents.push_back(obj);
    
    return true;
}


Node* Node::getChildAt(int position){
    return children[position];
}


std::vector<Node*> Node::getAllChildren(){
    return children;
}