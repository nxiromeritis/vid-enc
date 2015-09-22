#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static int rlps[64][4] =
{
    { 128, 176, 208, 240},
    { 128, 167, 197, 227},
    { 128, 158, 187, 216},
    { 123, 150, 178, 205},
    { 116, 142, 169, 195},
    { 111, 135, 160, 185},
    { 105, 128, 152, 175},
    { 100, 122, 144, 166},
    {  95, 116, 137, 158},
    {  90, 110, 130, 150},
    {  85, 104, 123, 142},
    {  81,  99, 117, 135},
    {  77,  94, 111, 128},
    {  73,  89, 105, 122},
    {  69,  85, 100, 116},
    {  66,  80,  95, 110},
    {  62,  76,  90, 104},
    {  59,  72,  86,  99},
    {  56,  69,  81,  94},
    {  53,  65,  77,  89},
    {  51,  62,  73,  85},
    {  48,  59,  69,  80},
    {  46,  56,  66,  76},
    {  43,  53,  63,  72},
    {  41,  50,  59,  69},
    {  39,  48,  56,  65},
    {  37,  45,  54,  62},
    {  35,  43,  51,  59},
    {  33,  41,  48,  56},
    {  32,  39,  46,  53},
    {  30,  37,  43,  50},
    {  29,  35,  41,  48},
    {  27,  33,  39,  45},
    {  26,  31,  37,  43},
    {  24,  30,  35,  41},
    {  23,  28,  33,  39},
    {  22,  27,  32,  37},
    {  21,  26,  30,  35},
    {  20,  24,  29,  33},
    {  19,  23,  27,  31},
    {  18,  22,  26,  30},
    {  17,  21,  25,  28},
    {  16,  20,  23,  27},
    {  15,  19,  22,  25},
    {  14,  18,  21,  24},
    {  14,  17,  20,  23},
    {  13,  16,  19,  22},
    {  12,  15,  18,  21},
    {  12,  14,  17,  20},
    {  11,  14,  16,  19},
    {  11,  13,  15,  18},
    {  10,  12,  15,  17},
    {  10,  12,  14,  16},
    {   9,  11,  13,  15},
    {   9,  11,  12,  14},
    {   8,  10,  12,  14},
    {   8,   9,  11,  13},
    {   7,   9,  11,  12},
    {   7,   9,  10,  12},
    {   7,   8,  10,  11},
    {   6,   8,   9,  11},
    {   6,   7,   9,  10},
    {   6,   7,   8,   9},
    {   2,   2,   2,   2}
};

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


static int renorm_table_lps[32] = {6,5,4,4,3,3,3,3,2,2,
            2,2,2,2,2,2,1,1,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1};

int debug = 0;


// only for debugging
static void trace(char str[], void *ptr, int nbits) {
	int i;
	int j;
	if (debug) {
		if (!strcmp(str, "qlps")) {
			printf("%s = {\n", str);
			for (i=0; i<64; i++)
				printf("\t%.4lf,\n", *((double *)ptr+i) );
			printf("};\n");
		}
		else if (!strcmp(str, "next_state")){
			printf("%s = {\n", str);
		}
	}
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

// for each state calculates lps probabilities for single-bit algorithm
void calc_qlps(double qlps[]) {
	double a = 0.949;
	int i;

	for (i=1; i<64; i++)
		qlps[i] = qlps[i-1]*a;

	trace("qlps", qlps, 0);
}


// for every state and bit combination of bits, stores the last state accessed
// by the corresponding bit stream
void calc_next_state(int *next_state[], int nbits){
	int i;
	int j;

	for(i=0; i<64; i++)
		for(j=0; j<pow(2,nbits); j++)
			next_state[i][j] = final_state(i, j, nbits);

	/*trace("next_state", next_state, nbits);*/

	for(i=0; i<64; i++) {
		printf("\t{ ");
		for(j=0; j<pow(2,nbits); j++) {
			printf("%4d,", next_state[i][j]);
		}
		printf(" },\n");
	}
	printf("};\n");
}
