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

        Node* root;
        Node* end;
        uint number_of_vertices=0;
        Graph graph_visualization;
        std::vector<Edge> edge_array;

    // public:
        CourseProgressionGraph(Node* rootp, Node* endp);

        Node* addNode(std::string student_id, Node* current_node, Node::Data data, int sequence_number, bool full_match);
        void wrap_up(std::string student_id, Node* current_node);
        
        void bfsParse();
        Node* bfsSearch(Node::Data data);
        bool valueIn(Node* val, std::vector<Node*> array);
        void initializeGraph();
        void printGraph();
};