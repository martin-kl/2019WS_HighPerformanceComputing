__kernel void diffusion (__global double *boxes, float diffusion_const) {
    
    //width and height with padding
    int width = get_global_size(0);
    int height = get_global_size(1);

    int ix = get_global_id(0);
    int jx = get_global_id(1);

    int px = jx*width + ix;
    double new_temp;

    //update temperature new_temp
    if (ix == 0 || ix == width-1 || jx == 0 || jx == height-1)
        new_temp = 0;
    else 
        new_temp = boxes[px] + diffusion_const * ( (boxes[px-1]+boxes[px+1]+boxes[px-width]+boxes[px+width])/4 - boxes[px] );

    //If barrier is inside a loop, all work-items in a work-group must
    //execute the barrier for each iteration of the loop before they 
    //are allowed to continue execution beyond the barrier.

    barrier(CLK_GLOBAL_MEM_FENCE);
    boxes[px] = new_temp;

    //The bug is: a work-group can synchronize internally, between work-items;
    //            but it cannot synchronize between work-groups.
    //In this program, it seems that there are more than one work-groups,
    //even with the barrier, the synchronization is still cannot be guarateed.
    //e.g. when updating the up-left corner box, the right side box has been
    //     updated, but the down side box hasn't.
    //So the way to solve is implementing a for-loop in host-side,
    //by iteratively invoking clEnqueueNDRangeKernel()
}
