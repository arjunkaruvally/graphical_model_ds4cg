#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "Node.h"
#include "Graph.h"
#include "CSVReader.cpp"

namespace logging = boost::log;

int main()
{
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );

    // Creating an object of CSVWriter
    CSVReader reader("/home/arjun/ssd/sps/data/fullDataSubset.csv");
    // Get the data from CSV File
    std::vector<std::vector<std::string> > df_full = reader.getData();
    
    // Get list of sps ids to check
    std::ifstream infile("/home/arjun/ssd/sps/data/spsid.txt");
    
    std::string sps_line;
    uint number_of_students=0;
    // Count lines for progress indication
    while (infile >> sps_line){
        ++number_of_students;
    }
    // Reset descriptor to beginning
    infile.clear();
    infile.seekg(0, infile.beg);

    int i=0;
    // Print header
    for(std::string data : df_full[0])
    {
       BOOST_LOG_TRIVIAL(debug)<<i<<" "<<data;
       i++;
    }
    BOOST_LOG_TRIVIAL(debug)<<"------------------------------";
    i=0;

    // Remove header
    df_full.erase(df_full.begin());

    // Create root node for flow graph
    Node root("START");
    Node end("END");

    // Create Graph with root and end
    Graph graph(&root, &end);

    // Read sps id list
    std::string a;

    BOOST_LOG_TRIVIAL(info)<<" Creating graph...";

    i=0;
    while (infile >> a)
    {
        BOOST_LOG_TRIVIAL(info)<<++i<<"/"<<number_of_students<<" : "<<a;
        std::vector<std::vector<std::string> > studentCourseData;
        // studentCourseData.push_back(df_full[0]);
        std::copy_if(df_full.begin(), df_full.end(), std::back_inserter(studentCourseData), [a](std::vector<std::string> row){return row[16].compare(a) == 0;});

        if(studentCourseData.size() > 0){
            //Add student to root node
            BOOST_LOG_TRIVIAL(trace) << "Addding student: "<<a;
            root.addStudent(a);
            
            std::vector<std::string> courseYears;
            int sequence_number = 0;
            bool make_transition = true;
            std::string current_year = studentCourseData[0][12];
            Node *current_node = &root;
            Node *prev_node = NULL;

            for(std::vector<std::string> course : studentCourseData){
                BOOST_LOG_TRIVIAL(trace)<<"Parsing course data "<<course[3];
                if(course[12].compare(current_year) != 0){
                    current_year = course[12];
                    sequence_number ++;
                    current_node = prev_node;
                    make_transition = true;
                }
                
                BOOST_LOG_TRIVIAL(trace)<<"Adding node...";
                prev_node = graph.addNode(a, current_node, course[3], sequence_number);
                BOOST_LOG_TRIVIAL(trace)<<"Add node complete";
                make_transition=false;
            
            }
            BOOST_LOG_TRIVIAL(trace)<<"Connecting END...";
            graph.wrap_up(a, prev_node);
            BOOST_LOG_TRIVIAL(trace)<<"END connection complete";
        }
        else{
            BOOST_LOG_TRIVIAL(fatal)<<"Courses filtered for students does not contain any record";
            throw;
        }
        if(i==20){
            break;
        }
        // break;    
    }
    BOOST_LOG_TRIVIAL(debug)<<"Creating graph Done";

    BOOST_LOG_TRIVIAL(debug)<<"Showing BFS output...";
    graph.bfsParse();
    BOOST_LOG_TRIVIAL(debug)<<"BFS output done...";

    BOOST_LOG_TRIVIAL(info)<<"Done";
    return 0;
}
