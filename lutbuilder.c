#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

static int next_state_mps[64] =
{
	1,2,3,4,5,6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,
    31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,
    51,52,53,54,55,56,57,58,59,60,
    61,62,62,63
};

static int next_state_lps[64] =
{
    0, 0, 1, 2, 2, 4, 4, 5, 6, 7,
    8, 9, 9,11,11,12,13,13,15,15,
    16,16,18,18,19,19,21,21,22,22,
    23,24,24,25,26,26,27,27,28,29,
    29,30,30,30,31,32,32,33,33,33,
    34,34,35,35,35,36,36,36,37,37,
    37,38,38,63
};

static unsigned int range[4] = {288, 352, 416, 480};
static unsigned int range_nbits[4];

int debug = 0;


// only for debugging
static void trace(char str[], ...) {
	int i;
	int j;
	int nbits;
	double *ptr1;
	int **ptr2;
	int **ptr3;
	int col_num;
	va_list ap;

	va_start(ap, str);

	if (debug) {
		if (!strcmp(str, "qlps")) {

			ptr1 = va_arg(ap, double *);
			printf("%s = {\n", str);

			for (i=0; i<64; i++)
				printf("\t%.4lf,\n", *(ptr1+i) );
			printf("};\n");

		}
		else if (!strcmp(str, "next_state") || !strcmp(str, "rxq_and_renorm")){

			ptr2 = va_arg(ap, int **);

			if (!strcmp(str, "rxq_and_renorm"))
				ptr3 = va_arg(ap, int **);

			nbits = va_arg(ap, int);

			printf("%s = {\n", str);

			col_num = (int)pow(2,nbits) << 2*(!strcmp(str, "rxq_and_renorm"));

			for(i=0; i<64; i++) {
				printf("\t{ ");
				for(j=0; j<col_num; j++) {
					printf("%6d,", ptr2[i][j]);
				}
				printf(" },\n");
			}
			printf("};\n");

			if (!strcmp(str, "rxq_and_renorm")) {
				printf("\n");
				for(i=0; i<64; i++) {
					printf("\t{ ");
					for(j=0; j<col_num; j++) {
						printf("%3d,", ptr3[i][j]);
					}
					printf(" },\n");
				}
				printf("};\n");
			}
		}
		else if (!strcmp(str, "missing_low")) {

			ptr2 = va_arg(ap, int **);
			nbits = va_arg(ap, int);

			printf("%s = {\n", str);

			for(i=0; i<64; i++) {
				printf("\t{ ");
				for(j=0; j<(int)pow(2,nbits)<<2; j++) {
					printf("%6d,", ptr2[i][j]);
				}
				printf(" },\n");
			}
			printf("};\n");

		}
		else {
			printf("\nInvalid str argument\n");
		}
	}

	va_end(ap);
}


// returns the final state after n bits
static int final_state(int state, int num, int nbits) {
	int i;
	int jstate = state;

	for (i=nbits; i>0; i--) {
		if ((num>>(i-1)) & 0x1)
			jstate = next_state_lps[jstate];
		else
			jstate = next_state_mps[jstate];
	}

	return(jstate);
}



static int shifts(int nbits) {
	int i,tmp;
	double min_prob = 0.0185;
	int min_range = 288;
	int extra_bits = 0;

	//We could just do minrange*minprob**(nbits-1) and the find the shift number
	//But(!) for large nbit values machine errors might change a lot the final
	//range value.
	i=0;
	tmp = min_range;
	while(i<nbits) {
		if ( round((double)tmp*min_prob) < 2 ) {
			while ( round((double)tmp*min_prob) < 2 ) {
				extra_bits++;
				min_range <<= 1;
				tmp = min_range;
			}
			i=0;
			continue;
		}
		else {
			i++;
			tmp = round((double)tmp*min_prob);
		}
	}
	return(extra_bits);
}


// for each state calculates lps probabilities for single-bit algorithm
void calc_qlps(double qlps[]) {
	double a = 0.949;
	int i;

	for (i=1; i<64; i++)
		qlps[i] = qlps[i-1]*a;

	trace("qlps", qlps);
}


// for every state and bit combination of bits, stores the last state accessed
// by the corresponding bit stream
void calc_next_state(int *next_state[], int nbits){
	int i;
	int j;

	for(i=0; i<64; i++)
		for(j=0; j<(int)pow(2,nbits); j++)
			next_state[i][j] = final_state(i, j, nbits);

	trace("next_state", next_state, nbits);

}

void calc_rxq_and_renorm(int *rxq[], int *renorm[], int nbits, double qlps[], int addbits) {
	int i,j,k,m;
	int state_tmp;
	double range_tmp;
	int extra_bits;

	//first we need to determine how long the range will be.
	extra_bits = shifts(nbits) + addbits;
	printf("\nNew Range:");
	for (i=0; i<4; i++) {
		range_nbits[i] = range[i] << extra_bits;
		printf("  %d  ", range_nbits[i]);

	}
	printf("\n");
	printf("\n%d bit implementation.\n", extra_bits);
	printf("Length of range in bits: %d\n", (10+extra_bits));
	printf("Minimum range value: \t%d\n", range[0]<<extra_bits);



	// iterate through states
	for (i=0; i<64; i++) {
		// for every nbit combination
		for (j=0; j<(int)pow(2,nbits); j++) {
			//and for every range value
			for (k=0; k<4; k++) {
				range_tmp = (double)range_nbits[k];
				state_tmp = i;
				// eventually build rxq[i][j+k] iterating though every bit of j
				for (m=nbits; m>0; m--) {
					if (! ((j>>(m-1))&0x1)) {		//mps
						range_tmp = range_tmp*(1-qlps[state_tmp]);
						state_tmp = next_state_mps[i];
					}
					else {		//lps
						range_tmp = range_tmp*qlps[state_tmp];
						state_tmp = next_state_lps[i];
					}
				}
				rxq[i][4*j+k] = round(range_tmp);
				renorm[i][4*j+k] = 0;
				while((rxq[i][4*j+k]<<renorm[i][4*j+k]) < ((int)pow(2,10+extra_bits)>>2)) {
					renorm[i][4*j+k]++;
				}
			}
		}
	}

	trace("rxq_and_renorm", rxq, renorm, nbits);
}


void calc_missing_low(int *missing_low[], int nbits, int *rxq[]) {
	int i,j,k,m;

	// fill the first set of 4
	for (i=0; i<64; i++) {
		for (j=0; j<4; j++) {
			missing_low[i][j]=0;
		}
	}

	for (i=0; i<64; i++) {
		for (j=1; j<(int)pow(2,nbits); j++) {
			for (k=0; k<4; k++) {
				missing_low[i][4*j+k] = 0;
				for (m=0; m<j; m++) {
					missing_low[i][4*j+k] += rxq[i][4*m+k];
				}
			}
		}
	}
	trace("missing_low", missing_low, nbits);
}
