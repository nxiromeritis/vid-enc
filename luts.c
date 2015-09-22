#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lutbuilder.h"

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

int **rxq;

int **next_state;

int main(int argc, char *argv[]) {
	int nbits = 2;
	int i;

	if (argc == 2)
		nbits = atoi(argv[1]);
	if (nbits < 2)
		nbits = 2;

	debug = 1;


	// store every state's lps probabilities
	calc_qlps(qlps);


	// store all the last state accessed given n bit stream
	next_state = (int **)malloc(64*sizeof(int *));
	for(i=0; i<64; i++)
		next_state[i] = (int *)malloc(pow(2,nbits)*sizeof(int));
	calc_next_state(next_state, nbits);


	// store each new range of the divided integral to rxq
	rxq = (int **)malloc(64*sizeof(int *));
	for (i=0; i<64; i++)
		rxq[i] = (int *)malloc(pow(2,nbits)*4*sizeof(int));
	calc_rxq(rxq, nbits, qlps);


	for(i=0; i<64; i++)
		free(next_state[i]);
	free(next_state);

	for (i=0; i<64; i++)
		free(rxq[i]);
	free(rxq);

	return(0);
}
