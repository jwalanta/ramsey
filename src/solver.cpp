#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include "bigint.h"
#include "solver.h"
#include "utils.h"

Solver::Solver(const char *folder){

    // save folder
    strcpy(this->folder, folder);

    // initialize input graphfiles
    input_graphs = new GraphFile(folder);

    // get MPI information
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_total_processes); 

    // Get my rank among all the processes
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_this_process);      
}

Solver::~Solver(){

}

void Solver::constraints_create(int s, int t, int n){

    // vector to store edges of complete graph
    std::vector<BIGINT> v;    

    // calculate edges
    int e=n*(n-1)/2, i;

    Constraint c;
    std::vector<int> tmp;    

    // computing minimum size of constraint
    int shift = (n-1)*(n-2)/2;

    // remove all unnecessary constraints
    BIGINT min_constraint = 1;
    min_constraint<<=shift;

    // get combinations for K_s 
    v.clear();
    tmp.clear();    
    get_combinations(v,n,s,tmp,-1);
    c.sign = '<';
    c.rhs = s*(s-1)/2;        
    for (i=0;i<v.size();++i){
        c.lhs = v[i];
        if (c.lhs >= min_constraint) constraint_add(c);
        //constraint_add(c);
    }


    // get combinations for K_t 
    v.clear();
    tmp.clear();
    get_combinations(v,n,t,tmp,-1);
    c.sign = '>';
    c.rhs = 0;
    for (i=0;i<v.size();++i){
        c.lhs = v[i];
        if (c.lhs >= min_constraint) constraint_add(c);
        //constraint_add(c);
    }    

}

void Solver::constraint_add(Constraint c){
    constraint.push_back(c);
}

void Solver::constraints_clear(){
    constraint.clear();
}

void Solver::constraints_print(){
    int l_count = 0, g_count = 0;
    std::cout << std::endl;

    for (int i=0;i<constraint.size();i++){
        //std::bitset<64> x(constraint[i].lhs);
        std::cout << "[" << binary_str(constraint[i].lhs,64) << "]" << constraint[i].sign << constraint[i].rhs << std::endl;

        if (constraint[i].sign=='<') l_count++; else g_count++;
    }

    std::cout << "<: " << l_count << " >: " << g_count << std::endl;
}

int Solver::check(BIGINT n){

    BIGINT t;

    // check n against all constraints
    for (int i=0;i<constraint.size();++i){

        t = constraint[i].lhs & n;

        if (constraint[i].sign == '<'){
            if (popcount(t) >= constraint[i].rhs){
                // doesnt satisfy
                return 1;
            }
        }
        
        if (constraint[i].sign == '>'){
            if (popcount(t) <= constraint[i].rhs){
                // doesnt satisfy
                return 2;
            }
        }

    }

    return 0;

}


void Solver::solve_ramsey(int s, int t){

    int n=2, total, total_tmp, i;
    clock_t start_time;

    // for gathering and merging files
    char pattern[255], command[255];
    glob_t globbuf;

    // note starting clock
    start_time = clock();

    // clear graphs vector
    graphs.clear();

    // initialize seed graph files
    if (mpi_this_process == 0){
        input_graphs->create_seed_graph_files(s, t);
        
        printf("R(%d,%d,1) = 1 [0s]\n", s, t);
        printf("R(%d,%d,2) = 2 [0s]\n", s, t);

    }

    // sync all processes at this point
    MPI_Barrier(MPI_COMM_WORLD);

    while (n++){

        // set current file prefix
        sprintf(current_file_prefix, "%sr_%d_%d_%d", folder, s, t, n);        

        // check if the file already exists
        sprintf(command, "%s.g6", current_file_prefix);
        if (file_exists(command)){

            if (mpi_this_process == 0){
                printf("R(%d,%d,%d) = skip [%ds]\n", s, t, n, 
                    (int)(double(clock() - start_time) / CLOCKS_PER_SEC));
            }

            continue;
        }

        // clear constraints
        constraints_clear();

        // create constraints
        constraints_create(s, t, n);

        //constraints_print();

        // initialize input graph files
        input_graphs->init(s, t, n-1);

        // reset output graph file count
        output_graphfile_count = 0;
        output_graphs_count = 0;

        // find ramsey graphs
        solve_using_edges(n);

        // total graphs computed in this process
        total = output_graphs_count;

        // wait up for all processes to finish up
        if (mpi_this_process == 0){

            // sum up all totals from other processes
            for (i=1;i<mpi_total_processes;i++){
                MPI_Recv(&total_tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                total += total_tmp;
            }

            printf("R(%d,%d,%d) = %d [%ds]\n", s, t, n, total, 
                (int)(double(clock() - start_time) / CLOCKS_PER_SEC));

        }
        else{
            // send total to process 0
            MPI_Send(&total, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

        // gather all files, sort and merge
        if (mpi_this_process == 0){

            int found = 0;

            if (total==0) {
                // no ramsey graphs found
                // current n is the Ramsey Number

                // stop rest of the processes
                found = 1;
                for (i=1;i<mpi_total_processes;i++){
                    MPI_Send(&found, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                }

                // delete files
                sprintf(command, "rm %s*.g6", current_file_prefix);
                int sc = system(command);                

                // stop
                break;
            }            

            // glob over all files that have been generated

            /*
            sprintf(pattern, "%sr_%d_%d*.g6", folder, s, t);

            glob(pattern, GLOB_TILDE, NULL, &globbuf);

            // sort all files individually
            for (i=0;i < globbuf.gl_pathc; ++i){

                sprintf(command, "LC_ALL=C sort -o '%s' '%s'", 
                    globbuf.gl_pathv[i], globbuf.gl_pathv[i]);
                int r = system(command);
            }

            if( globbuf.gl_pathc > 0 ) globfree( &globbuf );
            */

            // merge files (they are already sorted)
            sprintf(command, "LC_ALL=C sort -mu -o %s.g6 %s*.g6", 
                current_file_prefix, current_file_prefix);

            int sc = system(command);

            // delete individual files
            sprintf(command, "rm %s_*.g6", current_file_prefix);
            sc = system(command);

            // send msg to other processes to continue
            for (i=1;i<mpi_total_processes;i++){
                MPI_Send(&found, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }            

        }
        else{
            int found;
            MPI_Recv(&found, 1, MPI_INT, 0, 0, 
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);            

            // check if process 0 wants to continue for next n
            if (found) break;

        }
    }


}

void Solver::solve_using_edges(int vertices){

    // shift count for new edges
    int shift = (vertices-1)*(vertices-2)/2;

    // set initial pointer according to mpi_this_process
    long double graphs_per_process = input_graphs->get_total_graphs() / 
        (long double)mpi_total_processes;

    input_graphs->seek_to( (unsigned long long)(graphs_per_process * mpi_this_process) );

    unsigned long long total_graphs_to_process = (unsigned long long)(graphs_per_process * (mpi_this_process+1)) 
        - (unsigned long long)(graphs_per_process * mpi_this_process);

    // iterate over all previous graphs
    while (total_graphs_to_process--){
        add_edge(input_graphs->get_y(), 0, vertices, 0, shift);    
    }

    // write graph file
    char filename[255];
    sprintf(filename, "%s_%d_%d.g6", 
        current_file_prefix, mpi_this_process, output_graphfile_count);
    write_to_file_g6(&graphs, vertices, filename);

    output_graphs_count += graphs.size();
    graphs.clear();

}

void Solver::add_edge(BIGINT y, BIGINT edge, int vertices, int edge_start, int shift){

    if (edge_start>=vertices) return;

    BIGINT n, e;

    // prepare new graph using this edge
    n=edge;
    n<<=shift;
    n|=y;

    int c = check(n);
    if (c==0){
        // ramsey graph

        try{
            graphs.insert(canon_label(n,vertices));
        }
        catch(const std::bad_alloc& e){
            // out of memory,
            // write graph file
            char filename[255];
            sprintf(filename, "%s_%d_%d.g6", 
                current_file_prefix, mpi_this_process, output_graphfile_count++);
            write_to_file_g6(&graphs, vertices, filename);

            // clear graphs vector
            output_graphs_count += graphs.size();
            graphs.clear();
        }
    }
    else{
        // since this is not a ramsey graph
        // no point adding new edges

        // c==1 means the check failed for cliques
        // c==2 means it failed for independent set

        if (c==1) return;
    }

    for (int i=edge_start; i<vertices-1; ++i){

        // prepare edge
        e=1;
        e<<=i;

        add_edge(y, edge|e, vertices, i+1, shift);

    }

}

#ifndef MPZ_BIGINT

inline int Solver::popcount(BIGINT x){

    int pop_count;
    for (pop_count=0; x; pop_count++)
        x &= x-1;
    return pop_count;    
    
}

#else

inline int Solver::popcount(BIGINT x){
    return (int)mpz_popcount(x.get_mpz_t());
}

#endif