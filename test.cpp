#include <iostream>
#include <stdlib.h>
#include "utils.h"

int main(){

	BIGINT y;

	for (int i=0;i<50;i++){
		y=rand() % 1024;
		std::cout << binary_str(y, 10);

		y=canon_label(y,5);
		std::cout << binary_str(y, 10);
		std::cout << std::endl;
	}
	return 0;
}
