#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bigint.h"
#include "utils.h"
#include "graphfile.h"

GraphFile::GraphFile(const char* path){
	strcpy(folder, path);
	fp = NULL;
}

GraphFile::~GraphFile(){
	if (fp) fclose(fp);
}

void GraphFile::init(int s, int t, int n){

	char filename[255], buffer[255];

	// close file if open (by previous operation)
	if (fp) fclose(fp);

	sprintf(filename, "%sr_%d_%d_%d.g6",folder,s,t,n);

	fp = fopen(filename, "r");

	// compute stats for this graph file

	// get file size
	fseeko(fp, 0L, SEEK_END);
	file_length = (unsigned long long)ftello(fp);
	fseeko(fp, 0L, SEEK_SET);

	// get length of each line
	if (fgets(buffer, 255, fp) != NULL){
		line_length = strlen(buffer);
		fseeko(fp, 0L, SEEK_SET);
	}

	// calculate total graphs
	total_graphs = file_length / line_length;

	this->s = s;
	this->t = t;
	this->n = n;

	this->current_pos = 0;

}

void GraphFile::seek_to(unsigned long long pos){
	fseeko(fp, pos * line_length, SEEK_SET);
	current_pos = pos;
}

BIGINT GraphFile::get_y(){

	char buffer[256];

	if (fgets(buffer, 255, fp) != NULL){
		current_pos++;
	}

	return g6_to_y(buffer, n);
}

unsigned long long GraphFile::get_total_graphs(){
	return total_graphs;
}

unsigned long long GraphFile::get_current_pos(){
	return current_pos;
}

void GraphFile::create_seed_graph_files(int s, int t){

    char filename[255];

    // n=1
    sprintf(filename, "%sr_%d_%d_%d.g6",folder,s,t,1);
    if (!file_exists(filename)){
        FILE *fp;
        fp = fopen(filename, "w+");
        fputs("@\n", fp);
        fclose(fp);
    }

    // n=2
    sprintf(filename, "%sr_%d_%d_%d.g6",folder,s,t,2);
    if (!file_exists(filename)){
        FILE *fp;
        fp = fopen(filename, "w+");
        fputs("A?\nA_\n", fp);
        fclose(fp);
    }

}
