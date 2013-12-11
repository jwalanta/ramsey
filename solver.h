#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <gmpxx.h>

#ifndef SOLVER_H
#define SOLVER_H

//#define BIGINT unsigned long long
#define BIGINT __uint128_t
//#define BIGINT mpz_class
#define MAX_VERTEX 64

// precision for lhs part, PRECISION x 64 bits
#define PRECISION 1

extern int m2a_map[MAX_VERTEX][MAX_VERTEX];

struct Constraint{
    BIGINT lhs;
    char sign;
    int rhs;

    // for updating y
    BIGINT a;
    BIGINT b;
};

class Solver{

        std::vector<Constraint> constraint;
        std::set<BIGINT> old_graphs, new_graphs, *old_graphs_ptr, *new_graphs_ptr;
        int order;

    public:
        Solver();
        ~Solver();

        void add_constraint(Constraint c);
        void clear_constraints();
        void print_constraint();

        void solve(int vertices);
        bool check(BIGINT n);

        int popcount(BIGINT n);
        std::string get_g6(BIGINT n, int m);

        void solve_ramsey(int s, int t);
};

#endif