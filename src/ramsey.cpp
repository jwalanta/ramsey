#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include "bigint.h"
#include "solver.h"
#include "utils.h"

void usage(char* prog_name){
    std::cout << "Usage: " << prog_name << " s t folder" << std::endl;
}

int main_(){

    std::vector<BIGINT> v;    
    std::vector<int> tmp;    

    // get combinations for K_s 
    v.clear();
    tmp.clear();    

    int n = 6;
    int s = 3;

    get_combinations(v,n,s,tmp,-1);	

}

int main(int argc, char* argv[]){

	if (argc < 4) {
		usage(argv[0]);
		return 1;
	}

	// Initialize MPI
	MPI_Init(&argc, &argv);

	int s = atoi(argv[1]);
	int t = atoi(argv[2]);

	char folder[255];
	strcpy(folder, argv[3]);

	// check if string has trailing slash
	if (strrchr(folder, '/') - folder != strlen(folder) -1 ){
		strcat(folder, "/");
	}

	// create matrix to array map

	/*
	  0  1  2  3  4
	0 -  0  1  3  6         
	1    -  2  4  7     
	2       -  5  8    
	3          -  9   
	4             -  
	*/
	int i,j, count=0;
	for (j=1;j<MAX_VERTEX;j++){
	    for (i=0;i<j;i++){
	        m2a_map[i][j] = count++;
	    }
	}

		

	Solver so(folder);
	so.solve_ramsey(s,t);

	// shut down MPI
	MPI_Finalize(); 

	return 0;
}

