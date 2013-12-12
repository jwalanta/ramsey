#include <iostream>
#include <vector>
#include <gmpxx.h>
#include "nauty.h"

#ifndef UTILS_H
#define UTILS_H

void get_combinations(std::vector<BIGINT>& v, int n, int r, std::vector<int>& c, int st=-1);
void y_to_g6(BIGINT y, int n, char* s);
void y_to_graph(BIGINT y, int n, graph *g);
BIGINT g6_to_y(char* s);
BIGINT graph_to_y(graph *g, int n);
BIGINT canon_label(BIGINT y, int nv);

std::string binary_str(BIGINT n, int bits);

#endif