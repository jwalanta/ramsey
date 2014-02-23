#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include "solver.h"
#include "utils.h"

void usage(char* prog_name){
    std::cout << "Usage: " << prog_name << " s t [file_prefix]" << std::endl;
}

/*
int main(){
	BIGINT y;

	for (int i=0;i<50;i++){
		y=rand() % 1024;
		std::cout << binary_str(y, 10) << std::endl;

		y=canon_label(y,5);
		std::cout << binary_str(y, 10) << std::endl << std::endl;
	}
	return 0;	
}
*/

int main(int argc, char* argv[]){

	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	int s = atoi(argv[1]);
	int t = atoi(argv[2]);
	//int n = atoi(argv[3]);

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

	Solver so;
	so.solve_ramsey(s,t);

	return 0;
}

/*
int main(int argc, char* argv[]){

	vector<BIGINT> v;



	int i,j, count=0;
	int s=4, t=3, n=5;	


	int r;

	//for (n=1;n<=100;n++){

	cout << "R(" << s << "," << t << "," << n << ") = " << flush;

	// calculate edges
	int e=n*(n-1)/2;

	// create solver instance
	Solver so;
	Constraint c;
	vector<int> tmp;

	// get combinations for K_t	
	v.clear();
	tmp.clear();
	get_combinations(v,n,t,tmp);
	for (i=0;i<v.size();i++){
		c.lhs = v[i];
		c.sign = '>';
		c.rhs = 0;
		so.add_constraint(c);
	}

	// get combinations for K_s	
	v.clear();
	tmp.clear();	
	get_combinations(v,n,s,tmp);
	for (i=0;i<v.size();i++){
		c.lhs = v[i];
		c.sign = '<';
		c.rhs = s*(s-1)/2;
		so.add_constraint(c);
	}



	//so.print_constraint();

	//cout << "(computing..)" << flush;

	
	ostringstream ss;
	ss << "r_" << s << "_" << t << "_" << n << ".g6";
	string filename = ss.str();
	
	//cout << so.solve(n, filename) << endl;
	//r= so.solve(n, filename);
	r= so.solve(n, "");
	cout << r << endl;
	//if (r==0) break;
    //}

	return 0;

}

*/
