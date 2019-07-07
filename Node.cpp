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


bool Node::addStudents(std::vector<std::string> ids){
    for(std::string id : ids){
        BOOST_LOG_TRIVIAL(trace)<<"Pushing student id: "<<id<<"...";
        if(!memberOf(id, this->data.students)){
            this->data.students.push_back(id);
            BOOST_LOG_TRIVIAL(trace)<<"added to list";
            continue;
        }
        BOOST_LOG_TRIVIAL(trace)<<"duplicate exist";
    }
    return true;
}


bool Node::addSuccessfulStudent(std::string id){
    BOOST_LOG_TRIVIAL(trace)<<"Pushing successful student id: "<<id<<"...";
    if(!memberOf(id, this->data.students_successful)){
        this->data.students_successful.push_back(id);
        BOOST_LOG_TRIVIAL(trace)<<"added to list";
        return true;
    }
    BOOST_LOG_TRIVIAL(trace)<<"duplicate exist";
    return false;
}


bool Node::memberOf(std::string obj, std::vector<std::string> vec){
    BOOST_LOG_TRIVIAL(trace)<<"Calculating memberOf<string>";
    for(std::string member : vec){
        BOOST_LOG_TRIVIAL(trace)<<"comparing "<<member<<" and "<<obj;
        if(obj.compare(member)==0){
            return true;
        }
    }
    return false;
}


bool Node::memberOf(Node* obj, std::vector<Node*> vec){
    BOOST_LOG_TRIVIAL(trace)<<"Calculating memberOf<Node*>";
    for(Node* member : vec){
        BOOST_LOG_TRIVIAL(trace)<<"comparing "<<member->data.label<<"-"<<member->data.node_id<<" and "<<obj->data.label<<"-"<<obj->data.node_id;
        if(member->data.node_id==obj->data.node_id){
            return true;
        }
    }
    return false;
}

bool Node::addChildNode(Node* obj){
    BOOST_LOG_TRIVIAL(trace)<<"Adding child node: "<<obj->getLabel()<<"-"<<obj->data.node_id<<" in parent: "<<data.label<<"-"<<data.node_id<<"...";
    if(!memberOf(obj, children)){
        BOOST_LOG_TRIVIAL(trace)<<"Child Added";
        children.push_back(obj);
        obj->parents.push_back(this);
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