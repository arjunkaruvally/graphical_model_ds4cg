# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -std=c++11 -Wall -pedantic -g -O0 -DBOOST_LOG_DYN_LINK
LXXFLAGS = -lpthread -lboost_log -lboost_system -lboost_program_options

# ****************************************************
# Targets needed to bring the executable up to date

create_graph: create_graph.o Node.o CourseProgressionGraph.o utils.o
	$(CXX) utils.o Node.o CourseProgressionGraph.o create_graph.o $(LXXFLAGS) -o a.out

# The main.o target can be written more simply

create_graph.o: create_graph.cpp utils.h Node.h CourseProgressionGraph.h
	$(CXX) $(CXXFLAGS) -c utils.cpp Node.cpp CourseProgressionGraph.cpp create_graph.cpp

Node.o: Node.h

CourseProgressionGraph.o: CourseProgressionGraph.h Node.h utils.h