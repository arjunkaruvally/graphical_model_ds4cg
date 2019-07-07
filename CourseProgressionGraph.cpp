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
    vertex_dict.insert(std::pair<int, Node*>(0, root));
    vertex_dict.insert(std::pair<int, Node*>(1, end));   
}

// Find value in an array of values
bool CourseProgressionGraph::valueIn(Node* val, std::vector<Node*> array){
    return std::find(array.begin(), array.end(), val) != array.end();
}

// Think if this is required
std::vector<Node*> CourseProgressionGraph::getBfsParse(){
    // BFS show output the graph

    std::vector<Node*> result;
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
        
        result.push_back(s);

        queue.erase(queue.begin());

        for(Node* child : s->getAllChildren()){
            if(!this->valueIn(child, visited)){
                visited.push_back(child);
                queue.push_back(child);
            }
        }
    }

    BOOST_LOG_TRIVIAL(trace)<<"BFS Parse Done";
    return result;
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


Node* CourseProgressionGraph::addNode(std::string student_id, std::vector<Node*> current_nodes, Node::Data data, int sequence_number, bool full_match=false, bool successful=false){
    Node* node=NULL;
    BOOST_LOG_TRIVIAL(trace)<<"Step 1: Searching for existing nodes";
    ///////// Step 1: Search for the node

    if(!full_match){
        for(Node* current_node : current_nodes){
            std::vector<Node*> node_children=current_node->getAllChildren();
            for(uint i=0; i<node_children.size(); i++){
                Node* child=node_children[i];
                BOOST_LOG_TRIVIAL(trace)<<"Comparing "<<child->getLabel()<<" with "<<data.label;
                if(child->data.node_id==data.node_id){
                    BOOST_LOG_TRIVIAL(trace)<<"Match Found";
                    node=child;
                    break;
                }
            }
        }
    } else {
        node = bfsSearch(data);
    }

    BOOST_LOG_TRIVIAL(trace)<<"Step 2: Creating and adding node if not found";
    ///////// Step 2: create and add node to graph if not found
    if(node==NULL){
        BOOST_LOG_TRIVIAL(trace)<<"no node from step 1, creating new node for "<<data.label<<"-"<<data.node_id<<"...";
        node = new Node(data);

        //Sanitize dot file
        sanitizeDOTEntry(data.label);

        course_map.insert(std::pair<int, std::string>(data.node_id, data.label));
        vertex_dict.insert(std::pair<int, Node*>(data.node_id, node));
        number_of_vertices ++;
    }

    BOOST_LOG_TRIVIAL(trace)<<"adding student to node...";
    // Add student to node
    node->addStudents(std::vector<std::string>(1, student_id));
    if(successful) {
        node->addSuccessfulStudent(student_id);
    }
    
    BOOST_LOG_TRIVIAL(trace)<<"Step 3: Making changes to graph...";;

    //////// Step 3: do necessary modifications to graph
    for(Node* current_node : current_nodes){
        BOOST_LOG_TRIVIAL(trace)<<"Adding child to: "<<current_node->data.label<<"-"<<current_node->data.node_id;
        current_node->addChildNode(node);
    }

    BOOST_LOG_TRIVIAL(trace)<<"CourseProgressionGraph::addNode() done";
    // return pointer to node
    return node;
}


void CourseProgressionGraph::connectEND(std::string sid, std::vector<Node*> nodes, bool successful){
    BOOST_LOG_TRIVIAL(trace)<<"Connecting to END  for "<<sid;
    end->addStudents(std::vector<std::string>(1, sid));
    if(successful){
        end->addSuccessfulStudent(sid);
    }

    for(Node* node : nodes){
        if(node->addChildNode(end));
    }
}


void CourseProgressionGraph::wrap_up(){

    for(auto const& x : vertex_dict){
        Node* node=x.second;

        for(Node* child : node->children){
            BOOST_LOG_TRIVIAL(trace)<<"(wrap up)adding edge: "<<node->data.node_id<<" -> "<<child->data.node_id;
            edge_array.push_back(Edge(node->data.node_id, child->data.node_id));
        }

        if(node->data.label.compare("ROOT")!=0 &&
            node->data.label.compare("END")!=0 && 
            (node->children.size()==0 || 
            (node->children.size()==1 &&
             (node->children[0]->data.node_id == node->data.node_id))))
        {
            node->addChildNode(end);
            // edge_array.push_back(Edge(node->data.node_id, end->data.node_id));
        }
    }
}


void CourseProgressionGraph::initializeGraph(){
    BOOST_LOG_TRIVIAL(trace)<<"Intiializing boost graph. vertex count: "<<number_of_vertices;
    this->boost_graph = Graph(number_of_vertices);

    BOOST_LOG_TRIVIAL(debug)<<"Edges to add: "<<edge_array.size();
    for(Edge edge : edge_array){
        // BOOST_LOG_TRIVIAL(trace)<<"adding edge: "<<edge.first<<" -> "<<edge.second;
        // add_edge(edge.first, edge.second, this->boost_graph);
        // BOOST_LOG_TRIVIAL(trace)<<"adding edge done";
        // std::cout<<edge.first<<" -> "<<edge.second<<";"<<std::endl;
    }
    BOOST_LOG_TRIVIAL(trace)<<"Graph visualization done";
}


void CourseProgressionGraph::nodeDOT(std::ofstream &dot_file){
    std::vector<Node*> node_list = getBfsParse();

    dot_file<<"LEGEND"<<" [ shape=none"
                                <<", style=filled"
                                <<", fontcolor=black"
                                <<", fillcolor=\"#ffffff\""
                                <<", label=<"
                                <<"<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">"
                                    <<"<TR>"
                                        <<"<TD>course ID</TD><TD COLSPAN=\"2\">Course Name</TD>"
                                    <<"</TR>"
                                    <<"<TR>"
                                        <<"<TD>Total Successful</TD><TD>Total students</TD><TD>Success Rate</TD>"
                                    <<"</TR>"
                                <<"</TABLE>>"
                                <<"];"<<std::endl;

    for(auto const& x : vertex_dict){
        Node* s = x.second;
        sanitizeDOTEntry(s->data.label);
        float success_rate=((float)s->data.students_successful.size())/((float)s->data.students.size())*100;
        std::stringstream colour_code_stream;
        std::string colour_code;

        BOOST_LOG_TRIVIAL(trace)<<"Node: "<<s->data.label<<"_"<<s->data.node_id<<" success_rate: "<<success_rate<<" total_succ: "
                            <<s->data.students_successful.size()<<" total_students: "<<s->data.students.size();

        if(success_rate<=33){
            colour_code_stream<<"#";
            // colour_code_stream << "0 ";
            BOOST_LOG_TRIVIAL(trace)<<"colour index red: "<<(int)(255-(success_rate*255/33));
            // colour_code_stream << std::hex << (int)(255-(success_rate*255/33));
            // colour_code_stream << "0000";
            // int colour_index = (int)(100*(success_rate-67)/33);

            int colour_index = (int)(100*(success_rate-67)/33);
            colour_code_stream << "ff";
            if(colour_index < 16){
                colour_code_stream << "0";
            }
            colour_code_stream << std::hex << (int)(std::max(0, colour_index));
            if(colour_index < 16){
                colour_code_stream << "0";
            }
            colour_code_stream << std::hex << (int)(std::max(0, colour_index));

        } else if (success_rate<=66){
            colour_code_stream<<"#";
            BOOST_LOG_TRIVIAL(trace)<<"colour index green: "<<(int)((success_rate-33)*255/33);
            // colour_code_stream << std::hex << (int)((success_rate-33)*255/33);
            // colour_code_stream << "00";
            // int colour_index = (int)(100-(100*(success_rate-67)/33));

            int colour_index = (int)(100-(100*(success_rate-67)/33));
            if(colour_index < 16){
                colour_code_stream << "0";
            }
            colour_code_stream << "ff";
            colour_code_stream << std::hex << (int)(std::max(0, colour_index));
            if(colour_index < 16){
                colour_code_stream << "0";
            }
            colour_code_stream << std::hex << (int)(std::max(0, colour_index));

        } else {
            colour_code_stream<<"#";
            BOOST_LOG_TRIVIAL(trace)<<"colour index blue: "<<(int)((success_rate-67)/33);

            int colour_index = (int)(100-(100*(success_rate-67)/33));
            if(colour_index < 16){
                colour_code_stream << "0";
            }
            colour_code_stream << std::hex << (int)(std::max(0, colour_index));
            if(colour_index < 16){
                colour_code_stream << "0";
            }
            colour_code_stream << std::hex << (int)(std::max(0, colour_index));
            colour_code_stream << "ff";
        }
        colour_code = colour_code_stream.str();

        BOOST_LOG_TRIVIAL(trace)<<colour_code;
        
        // dot_file<<s->data.label<<"_"<<s->data.node_id<<" [ shape=none"
        //                         <<", style=filled"
        //                         <<", fontcolor=white"
        //                         <<", fillcolor=\""<<colour_code<<"\""
        //                         <<"];";

        dot_file<<s->data.label<<"_"<<s->data.node_id<<" [ shape=none"
                                <<", style=filled"
                                <<", fontcolor=white"
                                <<", fillcolor=\""<<colour_code<<"\""
                                <<", label=<"<<std::endl
                                <<"<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">"<<std::endl
                                    <<"<TR>"
                                        <<"<TD>"<<s->data.node_id<<"</TD><TD COLSPAN=\"2\">"<<s->data.label<<"</TD>"
                                    <<"</TR>"<<std::endl
                                    <<"<TR>"<<std::endl
                                        <<"<TD>"<<s->data.students_successful.size()<<"</TD>"<<std::endl
                                        <<"<TD>"<<s->data.students.size()<<"</TD>"<<std::endl
                                        <<"<TD>"<<success_rate<<"</TD>"<<std::endl
                                    <<"</TR>"<<std::endl
                                <<"</TABLE>>"
                                <<"];"<<std::endl;        
    }
}

void CourseProgressionGraph::printGraph(){
    BOOST_LOG_TRIVIAL(trace)<<"Graph to dot file....";
    std::ofstream dot_file("results/course_progression.dot");

    dot_file<<"digraph course{"<<std::endl;

    for(Edge edge : edge_array){
        dot_file<<course_map[edge.first]<<"_"<<edge.first<<" -> "<<course_map[edge.second]<<"_"<<edge.second<<";"<<std::endl;
    }

    // node specific dot values
    nodeDOT(dot_file);

    dot_file<<"}"<<std::endl;

    // write_graphviz(dot_file, graph_visualization);
    BOOST_LOG_TRIVIAL(trace)<<"Writing to dot file done";
}  