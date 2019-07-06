#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/graphviz.hpp>

#include "utils.h"
#include "Node.h"
#include "CourseProgressionGraph.h"

// using namespace boost;

// Constructor
CourseProgressionGraph::CourseProgressionGraph(Node* rootp, Node* endp){
    root = rootp;
    end = endp;
    number_of_vertices = 2;
    course_map.insert(std::pair<int, std::string>(0, "ROOT"));
    course_map.insert(std::pair<int, std::string>(1, "END"));
}

// Find value in an array of values
bool CourseProgressionGraph::valueIn(Node* val, std::vector<Node*> array){
    return std::find(array.begin(), array.end(), val) != array.end();
}

// Think if this is required
void CourseProgressionGraph::bfsParse(){
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

// Think if this is required
Node* CourseProgressionGraph::bfsSearch(Node::Data data){
    // BFS show output the graph
    BOOST_LOG_TRIVIAL(trace)<<"In BFS Search";

    std::vector<Node*> visited;
    std::vector<Node*> queue;
    Node* s=NULL;
    
    // Mark current node as visited and queue it

    BOOST_LOG_TRIVIAL(trace)<<"Pushing to queues...";
    visited.push_back(root);
    queue.push_back(root);

    BOOST_LOG_TRIVIAL(trace)<<"BFS Search Started...";

    while(queue.size()>0){
        s = queue[0];
        BOOST_LOG_TRIVIAL(trace)<<"Comparing "<<s->data.label<<":"<<s->data.node_id<<" and "<<data.label<<":"<<data.node_id;

        if(s->data.node_id == data.node_id){
            BOOST_LOG_TRIVIAL(trace)<<"BFS Parse Done with match found";
            return s;
        }

        queue.erase(queue.begin());

        for(Node* child : s->getAllChildren()){
            if(!this->valueIn(child, visited)){
                visited.push_back(child);
                queue.push_back(child);
            }
        }
    }

    BOOST_LOG_TRIVIAL(trace)<<"BFS Parse Done with no match found";
    return NULL;
}


Node* CourseProgressionGraph::addNode(std::string student_id, Node* current_node, Node::Data data, int sequence_number, bool full_match=false){
    Node* node=NULL;
    BOOST_LOG_TRIVIAL(trace)<<"Step 1: Searching for existing nodes";
    ///////// Step 1: Search for the node
    std::vector<Node*> node_children=current_node->getAllChildren();

    if(!full_match){
        for(uint i=0; i<node_children.size(); i++){
            Node* child=node_children[i];
            BOOST_LOG_TRIVIAL(trace)<<"Comparing "<<child->getLabel()<<" with "<<data.label;
            if(child->getLabel().compare(data.label)==0){
                BOOST_LOG_TRIVIAL(trace)<<"Match Found";
                node=child;
                break;
            }
        }
    } else {
        node = bfsSearch(data);
    }

    BOOST_LOG_TRIVIAL(trace)<<"Step 2: Creating and adding node if not found";
    ///////// Step 2: create and add node to graph if not found
    if(node==NULL){
        BOOST_LOG_TRIVIAL(trace)<<"no node from step 1, creating new node for "<<data.label<<"...";
        node = new Node(data);

        //Sanitize dot file
        findAndReplaceAll(data.label, "&", "_AND_");
        findAndReplaceAll(data.label, " ", "_");
        findAndReplaceAll(data.label, "/", "_OR_");

        course_map.insert(std::pair<int, std::string>(data.node_id, data.label));
        number_of_vertices ++;
    }

    BOOST_LOG_TRIVIAL(trace)<<"adding student to node...";
    // Add student to node
    node->addStudent(student_id);
    
    BOOST_LOG_TRIVIAL(trace)<<"Step 3: Making changes to graph...";;

    //////// Step 3: do necessary modifications to graph
    if(current_node->addChildNode(node)){
        edge_array.push_back(Edge(current_node->data.node_id, node->data.node_id));
    }
    

    BOOST_LOG_TRIVIAL(trace)<<"CourseProgressionGraph::addNode() done";
    // return pointer to node
    return node;
}


void CourseProgressionGraph::wrap_up(std::string student_id, Node* current_node){
    current_node->addChildNode(end);
    end->addStudent(student_id);

    edge_array.push_back(Edge(current_node->data.node_id, end->data.node_id));
}

void CourseProgressionGraph::initializeGraph(){
    BOOST_LOG_TRIVIAL(trace)<<"Creating graph visualization";
    graph_visualization = Graph(number_of_vertices);

    BOOST_LOG_TRIVIAL(debug)<<"Edges to add: "<<edge_array.size();
    for(Edge edge : edge_array){
        add_edge(edge.first, edge.second, graph_visualization);
        // std::cout<<edge.first<<" -> "<<edge.second<<";"<<std::endl;
    }
    BOOST_LOG_TRIVIAL(trace)<<"Graph visualization done";
}

void CourseProgressionGraph::printGraph(){
    BOOST_LOG_TRIVIAL(trace)<<"Graph to dot file....";
    std::ofstream dot_file("results/course_progression.dot");

    dot_file<<"digraph course{"<<std::endl;

    for(Edge edge : edge_array){
        dot_file<<course_map[edge.first]<<"_"<<edge.first<<" -> "<<course_map[edge.second]<<"_"<<edge.second<<";"<<std::endl;
    }

    dot_file<<"}"<<std::endl;

    // write_graphviz(dot_file, graph_visualization);
    BOOST_LOG_TRIVIAL(trace)<<"Writing to dot file done";
}  