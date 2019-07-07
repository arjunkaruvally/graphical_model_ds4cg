#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "Node.h"

using namespace boost;

// Definition
class CourseProgressionGraph
{
    public:
        typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
        typedef std::pair<int, int> Edge;
        std::map<int, std::string> course_map;
        std::map<int, int> success_map;
        std::map<int, Node*> vertex_dict;

        Node* root;
        Node* end;
        uint number_of_vertices=0;
        Graph boost_graph;
        std::vector<Edge> edge_array;

    // public:
        CourseProgressionGraph(Node* rootp, Node* endp);

        Node* addNode(std::string student_id, Node* current_node, Node::Data data, int sequence_number, bool full_match, bool successful);
        void wrap_up();
        
        std::vector<Node*> getBfsParse();
        Node* bfsSearch(Node::Data data);
        void nodeDOT(std::ofstream &dot_file);
        void bfsWrapUp();
        bool valueIn(Node* val, std::vector<Node*> array);
        void initializeGraph();
        void printGraph();
};