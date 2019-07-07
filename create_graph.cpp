#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "utils.h"
#include "Node.h"
#include "CourseProgressionGraph.h"
#include "CSVReader.cpp"

namespace logging = boost::log;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    // Number of students to consider
    uint subset_count = 99999;

    po::options_description desc("Options");
    desc.add_options()
        ("help", "Print help messages")
        ("debug", "Enable debug level logging; if multiple level present lowest is taken")
        ("trace", "Enable trace level logging(gruesome details); if multiple level present lowest is taken")
        ("subset_count,n", po::value<uint>(), "subset count of students to consider");

    int logging_level = logging::trivial::info;
    
    // Declare and parse command line argument
    po::variables_map params;
    po::store(po::parse_command_line(argc, argv, desc),  params);

    if( params.count("help") ) {
        BOOST_LOG_TRIVIAL(info)<<"This is help"<<std::endl<<desc<<std::endl;
        return 0;
    }

    if( params.count("debug") ) {
        logging_level = logging::trivial::debug;
    }

    if( params.count("trace") ) {
        logging_level = logging::trivial::trace;
    }

    if( params.count("subset_count")) {
        subset_count = params["subset_count"].as<uint>();
    }
    BOOST_LOG_TRIVIAL(info)<<"subset count: "<<subset_count;

    // Set logging levels
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging_level
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

    number_of_students = std::min(number_of_students, subset_count);

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
    Node::Data root_data=*(new Node::Data());
    root_data.label = "ROOT";
    root_data.node_id = 0;

    Node::Data end_data = *(new Node::Data());
    end_data.label = "END";
    end_data.node_id = 1;
    
    Node root(root_data);
    Node end(end_data);

    // Create CourseProgressionGraph with root and end
    CourseProgressionGraph graph(&root, &end);

    // Read sps id list
    std::string a;

    BOOST_LOG_TRIVIAL(info)<<" Creating graph...";

    i=0;
    while (infile >> a)
    {
        BOOST_LOG_TRIVIAL(info)<<++i<<"/"<<number_of_students<<" : "<<a<<" progress--------------------------------------";
        std::vector<std::vector<std::string> > studentCourseData;
        // studentCourseData.push_back(df_full[0]);
        std::copy_if(df_full.begin(), df_full.end(), std::back_inserter(studentCourseData), [a](std::vector<std::string> row){return row[16].compare(a) == 0;});

        if(studentCourseData.size() > 0){
            //Add student to root node
            BOOST_LOG_TRIVIAL(trace) << "Addding student to root: "<<a;
            root.addStudents(std::vector<std::string>(1, a));

            if(studentCourseData[0][31].compare("1")==0){
                root.addSuccessfulStudent(a);
            }
            
            std::vector<std::string> courseYears;
            int sequence_number = 0;
            bool make_transition = true;
            std::string current_year = studentCourseData[0][12];
            Node *current_node = &root;
            Node *prev_node = NULL;
            bool successful_student = false;

            for(std::vector<std::string> course : studentCourseData){
                successful_student = (course[31].compare("1")==0);
                
                BOOST_LOG_TRIVIAL(trace)<<"Parsing course data "<<course[3]<<" success: "<<course[31];
                
                for(int i=0; i<course.size(); i++){
                    BOOST_LOG_TRIVIAL(trace)<<i<<":"<<course[i]<<" ";
                }
                
                if(course[12].compare(current_year) != 0){
                    current_year = course[12];
                    sequence_number ++;
                    current_node = prev_node;
                    make_transition = true;
                }
                
                BOOST_LOG_TRIVIAL(trace)<<"Adding node...";
                Node::Data course_data = Node::Data();
                course_data.label = course[3];
                course_data.node_id = stoll(course[4]);

                prev_node = graph.addNode(a, current_node, course_data, sequence_number, true, successful_student);
                BOOST_LOG_TRIVIAL(trace)<<"Add node complete";
                make_transition=false;
            
            }
        }
        else{
            BOOST_LOG_TRIVIAL(fatal)<<"Courses filtered for students does not contain any record";
            throw;
        }

        if(i>=number_of_students){
            break;
        }
        // break;    
    }
    
    BOOST_LOG_TRIVIAL(trace)<<"Connecting END...";
    graph.wrap_up();
    BOOST_LOG_TRIVIAL(trace)<<"END connection complete";

    BOOST_LOG_TRIVIAL(debug)<<"Creating graph Done";

    // BOOST_LOG_TRIVIAL(debug)<<"Showing BFS output...";
    // graph.bfsParse();
    // BOOST_LOG_TRIVIAL(debug)<<"BFS output done...";

    graph.initializeGraph();

    BOOST_LOG_TRIVIAL(info)<<"Creating dot file...";
    graph.printGraph();

    BOOST_LOG_TRIVIAL(info)<<"Done";
    return 0;
}
