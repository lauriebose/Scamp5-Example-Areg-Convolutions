#include <scamp5.hpp>

#include "OUTPUT_AREG_BITSTACK.hpp"
#include "CONVOLUTION_FUNCS.hpp"

#include "weights.hpp"

using namespace SCAMP5_PE;


int main()
{
	vs_init();

	vs_stopwatch timer;

	const int display_size = 2;
    auto display_00 = vs_gui_add_display("image display_00",0,0,display_size);
    auto display_01 = vs_gui_add_display("cv1 display_01",0,display_size,display_size);

	int use_camera_img = 0;
	vs_gui_add_switch("use_camera_img: ",false,&use_camera_img);

	int camera_img_nudge = 0;
	vs_gui_add_slider("camera_img_nudge: ",-127,127,0,&camera_img_nudge);

	int kernel_index = 0;
	vs_gui_add_slider("kernel_index: ",0,15,0,&kernel_index);

	int convol_method = 0;
	vs_gui_add_slider("convol_method: ",0,2,1,&convol_method);

	int divisions = 0;
	vs_gui_add_slider("divisions: ",0,6,3,&divisions);

	int division_method = 0;
	vs_gui_add_slider("division_method: ",0,2,1,&division_method);

	int apply_relu = 0;
	vs_gui_add_switch("apply_relu: ",false,&apply_relu);

	int maxpooling = 0;
	vs_gui_add_slider("maxpooling: ",0,4,0,&maxpooling);

	int maxpool_blocking = 0;
	vs_gui_add_switch("maxpool_blocking: ",false,&maxpool_blocking);

	vs_gui_set_info(VS_M0_PROJECT_INFO_STRING);

    while(1){

    	vs_frame_loop_control();

    	if(use_camera_img)
    	{
    		int gain = vs_gui_read_slider(VS_GUI_FRAME_GAIN);
			scamp5_get_image(A,F,gain);
			scamp5_in(F,camera_img_nudge);
			scamp5_kernel_begin();
				add(A,A,F);
			scamp5_kernel_end();
    	}
    	else
    	{
    		//DRAW TEST IMAGE INTO REG A
    		int block_size = 32;
    		scamp5_kernel_begin();
				CLR(S6);
			scamp5_kernel_end();
    		scamp5_draw_begin(S6);
    			for(int y = 0 ; y < 256 ; y+=block_size)
    			{
    				for(int x = 0 ; x < 256 ; x+=block_size*2)
					{
    					if(x < 64 || x >= 192 || y < 64 || y >= 192)
    					{
    						int x2 = x;
							if((y/block_size)%2 == 0)
							{
								x2+=block_size;
							}
							scamp5_draw_rect(y,x2,y+block_size,x2+block_size);
    					}
					}
    			}

    			int rad = 48;
    			int thickness = 4;
    			for(int r = rad - thickness ; r <  rad + thickness; r++)
    			{
        			scamp5_draw_circle(127,127,r);
    			}
			scamp5_draw_end();

			scamp5_in(F,100);
			scamp5_in(A,-100);
			scamp5_kernel_begin();
				WHERE(S6);
					add(A,A,F);
					add(A,A,F);
				ALL();
				mov(F,A);
				add(A,A,F);
			scamp5_kernel_end();
    	}

    	//COPY IMG
		scamp5_kernel_begin();
			mov(B,A);
		scamp5_kernel_end();

		timer.reset();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (convol_method == 0)
		{
			switch(division_method)
			{
				case 0:
					for(int d = 0 ; d < divisions ; d++)
					{
						scamp5_kernel_begin();
							mov(F,B);
							divq(B,F);
						scamp5_kernel_end();
					}
					break;
				case 1:
					for(int d = 0 ; d < divisions ; d++)
					{
						scamp5_kernel_begin();
							mov(F,B);
							diva(B,E,F);
						scamp5_kernel_end();
					}
					break;
				case 2:
					for(int d = 0 ; d < divisions ; d++)
					{
						scamp5_kernel_begin();
							mov(F,B);
							div(B,E,F);
						scamp5_kernel_end();
					}
					break;
			}

			//METHOD THAT ASSUMES THE INPUT IMAGE IS PRE-DIVIDED TO AVOID SATURATION ISSUES
			PERFORM_CONVOLUTION_INTO_F_PREDIVIDED(B,4,weights[kernel_index][0]);
		}

		if (convol_method == 1)
		{
			//METHOD THAT INCLUDES DIVISIONS TO ATTEMPT TO AVOID SATURATION
			PERFORM_CONVOLUTION_INTO_F(B,4,1,weights[kernel_index][0],true);
		}

		if (convol_method == 2)
		{
			//OPTIZED METHOD GENERATED FROM PIOTRS PYTHON SCRIPT
			PYTHON_GENERATED_CONVOLUTION(B);
		}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if(apply_relu)
		{
			scamp5_in(F,0);
			scamp5_kernel_begin();
				where(B);
					mov(F,B);
				all();
				mov(B,F);
			scamp5_kernel_end();
		}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		MAXPOOL_REG_INTO_F(B,maxpooling,maxpool_blocking,true);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		int time = timer.get_usec();
		vs_post_text("time %d \n",time);

		scamp5_output_image(B,display_01);
		scamp5_output_image(A,display_00);
    }

	return 0;
}
