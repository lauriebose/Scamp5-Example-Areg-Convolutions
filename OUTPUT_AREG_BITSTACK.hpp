#include "scamp5.hpp"
using namespace SCAMP5_PE;

#ifndef OUTPUT_AREG_BITSTACK_HPP
#define OUTPUT_AREG_BITSTACK_HPP
	void output_areg_F_via_bitstack_DNEWS(areg_t reg,vs_handle display)
	{
		scamp5_in(E,127);
		scamp5_kernel_begin();
			CLR(RS,RW,RN,RE);

			where(F);
				MOV(RE,FLAG);
				NOT(RF,FLAG);
			WHERE(RF);
				add(F,F,E);
			all();

			div(D,C,E);
			mov(E,D);

			sub(D,F,E);
			where(D);
				MOV(RN,FLAG);
				mov(F,D);
			all();

			div(D,C,E);
			mov(E,D);

			sub(D,F,E);
			where(D);
				MOV(RW,FLAG);
				mov(F,D);
			all();

			div(D,C,E);
			mov(E,D);

			sub(D,F,E);
			where(D);
				MOV(RS,FLAG);
				mov(F,D);
			all();
		scamp5_kernel_end();

        scamp5_output_bitstack_begin(display,4);
        scamp5_output_bitstack_bit(RE);
        scamp5_output_bitstack_bit(RN);
        scamp5_output_bitstack_bit(RW);
        scamp5_output_bitstack_bit(RS);
        scamp5_output_bitstack_end();
	}

	void output_areg_via_bitstack_DNEWS(areg_t reg,vs_handle display)
	{
		scamp5_dynamic_kernel_begin();
			mov(F,reg);
		scamp5_dynamic_kernel_end();
		output_areg_F_via_bitstack_DNEWS(reg,display);
	}
#endif
