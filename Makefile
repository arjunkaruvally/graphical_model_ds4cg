# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -std=c++11 -Wall -pedantic -g -O0 -DBOOST_LOG_DYN_LINK
LXXFLAGS = -lpthread -lboost_log -lboost_system

# ****************************************************
# Targets needed to bring the executable up to date

create_graph: create_graph.o Node.o Graph.o
	$(CXX) Node.o Graph.o create_graph.o $(LXXFLAGS) -o a.out

# The main.o target can be written more simply

create_graph.o: create_graph.cpp Node.h Graph.h
	$(CXX) $(CXXFLAGS) -c Node.cpp Graph.cpp create_graph.cpp

Node.o: Node.h

Graph.o: Graph.h Node.h