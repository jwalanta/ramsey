#include <stdio.h>

typedef mpz_class BIGINT;

#include "graphfile.h"

int main(){

	GraphFile gf("/tmp/");

	gf.init(3,6,11);

	printf("Total graphs: %llu\n", gf.get_total_graphs());

	int pos;


	while (1){
		printf("Enter position: ");
		scanf("%d", &pos);

		if (pos == -1) break;

		gf.seek_to(pos);
		gf.get_y();

		printf("Current position: %llu\n\n", gf.get_current_pos());

	}


}