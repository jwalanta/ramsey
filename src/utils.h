#include <vector>
#include <set>
#include <stdio.h>
#include <gmpxx.h>
#include "bigint.h"
#include "nauty.h"

#ifndef UTILS_H
#define UTILS_H

void get_combinations(std::vector<BIGINT>& v, int n, int r, std::vector<int>& c, int st);
void y_to_g6(BIGINT y, int n, char* s);
void y_to_graph(BIGINT y, int n, graph *g);
BIGINT g6_to_y(char* s, int n);
BIGINT graph_to_y(graph *g, int n);
BIGINT canon_label(BIGINT y, int nv);

std::string binary_str(BIGINT n, int bits);

int popcount(BIGINT x);

bool file_exists(const char* filename);

void write_to_file_g6(std::set<BIGINT> *graphs_ptr, int vertices, const char* filename);

#endif