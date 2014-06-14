#include <iostream>
#include <vector>
#include <set>
#include <stdio.h>
#include <gmpxx.h>
#include "bigint.h"

#ifndef GRAPHFILE_H
#define GRAPHFILE_H

class GraphFile{
	    FILE *fp;
	    unsigned long long file_length, current_pos, total_graphs;
	    int line_length;
	    char folder[255];

	    int s, t, n;

	public:

		GraphFile(const char* path);
		~GraphFile();

		void init(int s, int t, int n);
		void create_seed_graph_files(int s, int t);
		void seek_to(unsigned long long pos);
		BIGINT get_y();
		unsigned long long get_total_graphs();
		unsigned long long get_current_pos();
};

#endif