#include <iostream>
#include <vector>
#include <set>
#include <ctime>
#include <stdio.h>
#include <gmpxx.h>
#include <mpi.h>
#include "bigint.h"
#include "graphfile.h"

#ifndef SOLVER_H
#define SOLVER_H

extern int m2a_map[MAX_VERTEX][MAX_VERTEX];

struct Constraint{
    BIGINT lhs;
    char sign;
    int rhs;

    // for updating y
    //BIGINT a;
    //BIGINT b;
};

class Solver{

        // constraints
        std::vector<Constraint> constraint;

        // computed new graphs
        std::set<BIGINT> graphs;

        // input graph file
        GraphFile* input_graphs;

        // mpi variables
        int mpi_total_processes;
        int mpi_this_process;

        // current working folder
        char folder[255];
        char current_file_prefix[255];

        // output graph file count
        int output_graphfile_count;
        int output_graphs_count;

    public:
        Solver(const char *folder);
        ~Solver();

        // constraints
        void constraints_create(int s, int t, int n);
        void constraints_clear();
        void constraints_print();
        void constraint_add(Constraint c);
        int check(BIGINT n);

        // solve ramsey
        void solve_ramsey(int s, int t);
        void solve_using_edges(int vertices);
        void add_edge(BIGINT y, BIGINT edge, int vertices, int edge_start, int shift);

        // popcount
        int popcount(BIGINT x);

};

#endif