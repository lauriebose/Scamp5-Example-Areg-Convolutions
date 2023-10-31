#include <scamp5.hpp>
#include <vector>;
using namespace SCAMP5_PE;

//USES E,F
void PERFORM_CONVOLUTION_INTO_F_PREDIVIDED(areg_t reg, int convolution_size, const int8_t *kernel)
{
	scamp5_in(E,0);
	scamp5_dynamic_kernel_begin();
		mov(F,reg);
	scamp5_dynamic_kernel_end();
	for(int y = 0 ; y < convolution_size ; y++)
	{
		for(int x = 0 ; x < convolution_size ; x++)
		{
			int weight = kernel[y*convolution_size + x];
			if(y%2 == 1)
			{
				weight = kernel[y*convolution_size + convolution_size - 1 - x];
			}

			if(weight == -1)
			{
				scamp5_kernel_begin();
					sub(E,E,F);
				scamp5_kernel_end();
			}
			else
			{
				if(weight == 1)
				{
					scamp5_kernel_begin();
						add(E,E,F);
					scamp5_kernel_end();
				}
			}

			if(x != convolution_size -1)
			{
				//MOVE F IN ZIG ZAG
				if(y%2 == 0)
				{
					scamp5_kernel_begin();
						bus(NEWS,F);
						bus(F,XE);
					scamp5_kernel_end();
				}
				else
				{
					scamp5_kernel_begin();
						bus(NEWS,F);
						bus(F,XW);
					scamp5_kernel_end();
				}
			}
		}

		if(y != convolution_size -1)
		{
			scamp5_kernel_begin();
				bus(NEWS,F);
				bus(F,XN);
			scamp5_kernel_end();
		}
	}

	scamp5_dynamic_kernel_begin();
		mov(reg,E);
	scamp5_dynamic_kernel_end();
}

//USES D,E,F
void PERFORM_CONVOLUTION_INTO_F(areg_t reg, int convolution_size, int step_size,const int8_t *kernel, bool initial_division = true, bool copy_result_back_into_reg = true)
{
	scamp5_in(D,0);

	for(int y = 0 ; y < convolution_size ; y++)
	{
		if(initial_division)
		{
			scamp5_dynamic_kernel_begin();
					divq(E,reg);
					divq(F,E);
			scamp5_dynamic_kernel_end();
		}
		else
		{
			scamp5_dynamic_kernel_begin();
				mov(F,reg);
			scamp5_dynamic_kernel_end();
		}


		for(int s = 0 ; s < step_size*y ; s++)
		{
			scamp5_kernel_begin();
				bus(NEWS,F);
				bus(F,XN);
			scamp5_kernel_end();
		}

		scamp5_in(E,0);
		for(int x = 0 ; x < convolution_size ; x++)
		{
			int weight = kernel[y*convolution_size+x];

			if(weight == -1)
			{
				scamp5_kernel_begin();
					sub(E,E,F);
				scamp5_kernel_end();
			}
			else
			{
				if(weight == 1)
				{
					scamp5_kernel_begin();
						add(E,E,F);
					scamp5_kernel_end();
				}
			}

			if(x != convolution_size -1)
			{
				for(int s = 0 ; s < step_size ; s++)
				{
					scamp5_kernel_begin();
						bus(NEWS,F);
						bus(F,XE);
					scamp5_kernel_end();
				}
			}
		}

		scamp5_kernel_begin();
			divq(F,E);
			add(D,D,F);
		scamp5_kernel_end();

	}

	scamp5_kernel_begin();
		mov(F,D);
	scamp5_kernel_end();
	if(copy_result_back_into_reg)
	{
		scamp5_dynamic_kernel_begin();
			mov(reg,D);
		scamp5_dynamic_kernel_end();
	}
}

//USES A,B,D
void PYTHON_GENERATED_CONVOLUTION(areg_t reg)
{
	scamp5_dynamic_kernel_begin();
		mov(B,reg);
	scamp5_dynamic_kernel_end();

	scamp5_kernel_begin();
		 //assumes binary input already in B - (0,20)

			//d = 0 -- accumulates conv result
			res(D);

			//accumulate to A. A=A(west)+k --  add B (k=1) or subtract B (k=-1), then shift

		  // row 0
			bus(NEWS);
			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, NEWS);
			//divide a=a/4. accumulate to D
			diva(A, E, F);
			diva(A, E, F);
			//add(D, D, A, XN); //accumulate & shift D = D(north) + A
			bus (NEWS, D);
			bus (D, A, XN);

		  // row 1
			bus(NEWS);
			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, XW);
			bus(NEWS, A, B); // -1

			bus(A, NEWS);
			//divide a=a/4. accumulate to D
			diva(A, E, F);
			diva(A, E, F);
			//add(D, D, A, XN); //accumulate & shift D = D(north) + A
			bus (NEWS, D);
			bus (D, A, XN);

		  // row 2
			bus(NEWS);
			bus(A, XW);
			bus(NEWS, A);

			bus(A, XW);
			bus(NEWS, A);

			bus(A, XW);
			bus(NEWS, A);

			bus(A, XW);
			bus(NEWS, A);

			bus(A, XW);
			bus(NEWS, A);

			bus(A, NEWS);
			//divide a=a/4. accumulate to D
			diva(A, E, F);
			diva(A, E, F);
			//add(D, D, A, XN); //accumulate & shift D = D(north) + A
			bus (NEWS, D);
			bus (D, A, XN);

		  // row 3
			bus(NEWS);
			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, NEWS);
			//divide a=a/4. accumulate to D
			diva(A, E, F);
			diva(A, E, F);
			//add(D, D, A, XN); //accumulate & shift D = D(north) + A
			bus (NEWS, D);
			bus (D, A, XN);

		  // row 4
			bus(NEWS);
			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, XW, B); //  1
			bus(NEWS, A);

			bus(A, NEWS);
			//divide a=a/4. accumulate to D
			diva(A, E, F);
			diva(A, E, F);
			//add(D, D, A, XN); //accumulate & shift D = D(north) + A
			bus (NEWS, D);
			bus (D, A, XN);
	scamp5_kernel_end();

	scamp5_dynamic_kernel_begin();
		mov(reg,D);
	scamp5_dynamic_kernel_end();
}


void MAXPOOL_REG_INTO_F_DYNKER(areg_t reg, int maxpooling, int blocking, bool move_result_into_passed_reg)
{
	int maxpooling_size = 1 << maxpooling;

	scamp5_dynamic_kernel_begin();
		mov(E,reg);
	scamp5_dynamic_kernel_end();

	scamp5_kernel_begin();
		mov(F,E);
		for(int x = 0 ; x < 16 ; x++)
		{
				bus(NEWS,F);
				bus(F,XE);

				//SUE, STORE FLAG, REVERT WITH ADD, SAVES USING EXTRA AREG
				sub(F,F,E);
				where(F);
					MOV(S6,FLAG);
				all();
				add(F,F,E);

				WHERE(S6);
					mov(E,F);
				all();
		}
		scamp5_kernel_end();

	if(blocking)
	{
		scamp5_load_pattern(S6,0,maxpooling_size-1,255,256-maxpooling_size);
		scamp5_kernel_begin();
			mov(F,E);

			for(int x = 0 ; x < 16 ; x++)
			{
					WHERE(S6);
						mov(E,F);
					all();

					bus(NEWS,F);
					bus(F,XW);

					MOV(S5,S6);
					DNEWS(S6,S5,west);
			}
			scamp5_kernel_end();
	}


	scamp5_kernel_begin();
		mov(F,E);
		for(int y = 0 ; y < 16 ; y++)
		{
				bus(NEWS,F);
				bus(F,XN);

				//SUE, STORE FLAG, REVERT WITH ADD, SAVES USING EXTRA AREG
				sub(F,F,E);
				where(F);
					MOV(S6,FLAG);
				all();
				add(F,F,E);

				WHERE(S6);
					mov(E,F);
				all();
		}
		scamp5_kernel_end();

	if(blocking)
	{
		scamp5_load_pattern(S6,maxpooling_size-1,0,256-maxpooling_size,255);
		scamp5_kernel_begin();
			mov(F,E);
			for(int y = 0 ; y < 16 ; y++)
			{
					WHERE(S6);
						mov(E,F);
					all();

					bus(NEWS,F);
					bus(F,XS);

					MOV(S5,S6);
					DNEWS(S6,S5,south);
			}
			scamp5_kernel_end();
	}

	scamp5_kernel_begin();
		mov(F,E);
	scamp5_kernel_end();
	if(move_result_into_passed_reg)
	{
		scamp5_dynamic_kernel_begin();
			mov(reg,E);
		scamp5_dynamic_kernel_end();
	}
}


void MAXPOOL_REG_INTO_F(areg_t reg, int maxpooling, int blocking, bool move_result_into_passed_reg)
{
	int maxpooling_size = 1 << maxpooling;

	scamp5_dynamic_kernel_begin();
		mov(E,reg);
	scamp5_dynamic_kernel_end();

	scamp5_kernel_begin();
		mov(F,E);
	scamp5_kernel_end();
	for(int x = 0 ; x < maxpooling_size ; x++)
	{
		scamp5_kernel_begin();
			bus(NEWS,F);
			bus(F,XE);

			//SUE, STORE FLAG, REVERT WITH ADD, SAVES USING EXTRA AREG
			sub(F,F,E);
			where(F);
				MOV(S6,FLAG);
			all();
			add(F,F,E);

			WHERE(S6);
				mov(E,F);
			all();
		scamp5_kernel_end();
	}

	if(blocking)
	{
		scamp5_load_pattern(S6,0,maxpooling_size-1,255,256-maxpooling_size);
		scamp5_kernel_begin();
			mov(F,E);
		scamp5_kernel_end();
		for(int x = 0 ; x < maxpooling_size ; x++)
		{
			scamp5_kernel_begin();
				WHERE(S6);
					mov(E,F);
				all();

				bus(NEWS,F);
				bus(F,XW);

				MOV(S5,S6);
				DNEWS(S6,S5,west);
			scamp5_kernel_end();
		}
	}


	scamp5_kernel_begin();
		mov(F,E);
	scamp5_kernel_end();
	for(int y = 0 ; y < maxpooling_size ; y++)
	{
		scamp5_kernel_begin();
			bus(NEWS,F);
			bus(F,XN);

			//SUE, STORE FLAG, REVERT WITH ADD, SAVES USING EXTRA AREG
			sub(F,F,E);
			where(F);
				MOV(S6,FLAG);
			all();
			add(F,F,E);

			WHERE(S6);
				mov(E,F);
			all();
		scamp5_kernel_end();
	}

	if(blocking)
	{
		scamp5_load_pattern(S6,maxpooling_size-1,0,256-maxpooling_size,255);
		scamp5_kernel_begin();
			mov(F,E);
		scamp5_kernel_end();
		for(int y = 0 ; y < maxpooling_size ; y++)
		{
			scamp5_kernel_begin();
				WHERE(S6);
					mov(E,F);
				all();

				bus(NEWS,F);
				bus(F,XS);

				MOV(S5,S6);
				DNEWS(S6,S5,south);
			scamp5_kernel_end();
		}
	}

	scamp5_kernel_begin();
		mov(F,E);
	scamp5_kernel_end();
	if(move_result_into_passed_reg)
	{
		scamp5_dynamic_kernel_begin();
			mov(reg,E);
		scamp5_dynamic_kernel_end();
	}
}

