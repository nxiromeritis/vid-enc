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

int **renorm;

int **missing_low;

int main(int argc, char *argv[]) {
	int nbits = 2;
	int i;

	if (argc == 2)
		nbits = atoi(argv[1]);
	if (nbits < 2)
		nbits = 2;

	debug = 1;

	next_state = (int **)malloc(64*sizeof(int *));
	rxq = (int **)malloc(64*sizeof(int *));
	renorm = (int **)malloc(64*sizeof(int *));
	missing_low = (int **)malloc(64*sizeof(int *));
	for(i=0; i<64; i++) {
		next_state[i] = (int *)malloc((int)pow(2,nbits)*sizeof(int));
		rxq[i] = (int *)malloc((int)pow(2,nbits)*4*sizeof(int));
		renorm[i] = (int *)malloc((int)pow(2,nbits)*4*sizeof(int));
		missing_low[i] = (int *)malloc((int)pow(2, nbits)*4*sizeof(int));
	}


	// store every state's lps probabilities to qlps
	calc_qlps(qlps);

	// store all the last state accessed given n bit stream to next_state
	calc_next_state(next_state, nbits);
	// store each new range of the divided integral and correspondig renorm,
	// to rxq and renorm respectively
	calc_rxq_and_renorm(rxq, renorm, nbits, qlps);

	// store new low positions to missing_low
	calc_missing_low(missing_low, nbits, rxq);


	for(i=0; i<64; i++) {
		free(next_state[i]);
		free(rxq[i]);
		free(renorm[i]);
		free(missing_low[i]);
	}
	free(next_state);
	free(rxq);
	free(renorm);
	free(missing_low);

	return(0);
}
