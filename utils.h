#include <iostream>
#include <vector>
#include <gmpxx.h>

#ifndef UTILS_H
#define UTILS_H

void get_combinations(std::vector<BIGINT>& v, int n, int r, std::vector<int>& c, int st=-1);
void y_to_g6(BIGINT y, int n, char* s);
BIGINT g6_to_y(char* s);
BIGINT canon_label(BIGINT y, int nv);

std::string binary_str(BIGINT n, int bits);

#endif