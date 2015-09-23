#ifndef __LUTBUILDER_H__
#define __LUTBUILDER_H__

extern short int debug;

extern void calc_qlps();
extern void calc_next_state();
extern void calc_rxq_and_renorm();
extern void calc_missing_low();

#endif
