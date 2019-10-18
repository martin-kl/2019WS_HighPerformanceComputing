__kernel void diffusion (__global double *heat_values, float diffusion_const) {
    //width and height without padding
    int width = get_global_size(0);
    int height = get_global_size(1);

    int ix = get_global_id(0);
    int jx = get_global_id(1);

    int px = jx*width + ix;
    double new_temp;

    new_temp = heat_values[px] + diffusion_const * ( 
                (heat_values[px-1] + heat_values[px+1] + 
                    heat_values[px-width] + heat_values[px+width]
                )/4 - heat_values[px]);

    //wait for others
    barrier(CLK_GLOBAL_MEM_FENCE);
    heat_values[px] = new_temp;
}
