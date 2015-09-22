#include <stdio.h>
#include "lutbuilder.h"
#include <stdlib.h>
#include <math.h>

double qlps[64] = {
	0.5, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};

int **next_state;

int main(int argc, char *argv[]) {
	int nbits = 2;
	int i;

	if (argc == 2)
		nbits = atoi(argv[1]);
	if (nbits < 2)
		nbits = 2;

	debug = 1;
	calc_qlps(qlps);

	next_state = (int **)malloc(64*sizeof(int *));
	for(i=0; i<64; i++)
		next_state[i] = (int *)malloc(pow(2,nbits)*sizeof(int));
	calc_next_state(next_state, nbits);


	for(i=0; i<64; i++)
		free(next_state[i]);
	free(next_state);

	return(0);
}
