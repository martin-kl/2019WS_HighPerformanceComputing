#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <CL/cl.h>

#define DIFFUSION_FILE "diffusion"
#define KERNEL_FILE "kernel.cl"

//--    Prototypes              ///////////////////////////////////////////////////

void parseArguments(int argc, char *argv[], char *progname, unsigned int *iterations, float *diffusion_const);

long convertToInt(char *arg);

//--    main()              ///////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    char *progname;
    FILE *fp_diffusion;
    FILE *fp_kernel;

    //user input values
    unsigned int iterations;
    float diffusion_const;

    //variables
    double avg, avg_diff;
    size_t width, height, nmb_points, nmb_points_padding;
    cl_double *heat_values;
    char *kernel_code;

    // OpenCL specific variables
    cl_int error, status;
    cl_platform_id platform_id;
    cl_uint nmb_platforms;
    cl_device_id device_id;
    cl_uint nmb_devices;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;


    //get program name
    if (argc > 0)
        progname = argv[0];
    else
    {
        fprintf(stderr, "Error: no program name can be found\n");
        exit(EXIT_FAILURE);
    }

    parseArguments(argc, argv, progname, &iterations, &diffusion_const);
    //printf("Called with %d iterations and diffusion const of %f\n", iterations, diffusion_const);

    if ((fp_diffusion = fopen(DIFFUSION_FILE, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open \"%s\" file to read! Exiting.\n", DIFFUSION_FILE);
        exit(EXIT_FAILURE);
    }

    if(fscanf(fp_diffusion, "%lu %lu\n", &width, &height) != 2) {
        fprintf(stderr, "Cannot read width and height from diffusion file!\n");
        exit(EXIT_FAILURE);
    }
    //printf("width = %lu, height = %lu\n", width, height);

    //calloc heat values
    nmb_points = width * height;
    width += 2;
    height += 2;
    nmb_points_padding = width * height;

    heat_values = (cl_double *) malloc(nmb_points_padding* sizeof(cl_double));
    if(heat_values == NULL) {
        fprintf(stderr, "Error on allocating memory for the heat values\n");
        exit(EXIT_FAILURE);
    }
    long unsigned row, col;
    double value;
    while (!feof(fp_diffusion)) {
        if(fscanf(fp_diffusion, "%lu %lu %lf\n", &col, &row, &value) != 3) {
            fprintf(stderr, "Error while parsing input values from diffusion file!\n");
            exit(EXIT_FAILURE);
        }
        heat_values[(row+1) * width + col+1] = value;
    }
    fclose(fp_diffusion);


    // #### #### #### basic init done, now create stuff for OpenCL #### #### #### ####


    // read kernel code into a string
    if ((fp_kernel = fopen(KERNEL_FILE, "r")) == NULL) {
        fprintf(stderr, "Failed to open kernel file %s!\n", KERNEL_FILE);
        exit(EXIT_FAILURE);
    }

    size_t src_size;
    //go to end of file
    fseek(fp_kernel, 0, SEEK_END);
    //get number of characters
    src_size = ftell(fp_kernel);
    //go back to beginning of file
    fseek(fp_kernel, 0, SEEK_SET);
    //allocate memory

    kernel_code = (char *) malloc(src_size * sizeof(char));
    if (kernel_code == NULL) {
        fprintf(stderr, "Could not allocate memory for kernel!\n");
        exit(EXIT_FAILURE);
    }
    src_size = fread(kernel_code, sizeof(char), src_size, fp_kernel);
    fclose(fp_kernel);

    //get platform information
    if (clGetPlatformIDs(1, &platform_id, &nmb_platforms) != CL_SUCCESS) {
        fprintf(stderr, "cannot get platform\n");
        exit(EXIT_FAILURE);
    }

    //get device information
    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &nmb_devices) != CL_SUCCESS) {
        fprintf(stderr, "cannot get device\n");
        exit(EXIT_FAILURE);
    }

    //create opencl context
    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id, 0};
    context = clCreateContext(properties, 1, &device_id, NULL, NULL, &error);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot create context\n");
        exit(EXIT_FAILURE);
    }

    //create a command queue
    command_queue = clCreateCommandQueue(context, device_id, 0, &error);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot create commamd queue\n");
        exit(EXIT_FAILURE);
    }

    //create program from kernel source
    program = clCreateProgramWithSource(context, 1, (const char **) &kernel_code, 
          (const size_t *) &src_size, &error);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot create program object\n");
        exit(EXIT_FAILURE);
    }

    //build the program
    //we can pass a target device, if we don't do it, it will automatically take the one from program
    error = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    //error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "Cannot build program. log:\n");
        size_t log_size = 0;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        char *log = calloc(log_size, sizeof(char));
        if (log == NULL) {
            fprintf(stderr, "Could not allocate memory for log!\n");
            exit(EXIT_FAILURE);
        }
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        fprintf(stderr, "%s\n", log);
        free(log);
        exit(EXIT_FAILURE);
    }

    //create opencl kernel
    cl_kernel kernel;
    kernel = clCreateKernel(program, "diffusion", &error);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot create kernel\n");
        exit(EXIT_FAILURE);
    }

    // #### #### #### #### Kernel created, now create buffers #### #### #### #### ####

    //create memory buffers on the GPU
    cl_mem buffer_values = clCreateBuffer(context, CL_MEM_READ_WRITE,
          sizeof(cl_double) * nmb_points_padding, NULL, &error);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot create GPU memory buffer for heat values\n");
        exit(EXIT_FAILURE);
    }

    //copy to the respective memory buffers
    error = clEnqueueWriteBuffer(command_queue, buffer_values, CL_TRUE, 0,
          sizeof(cl_double) * nmb_points_padding, heat_values, 0, NULL, NULL);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot write heat values to input buffer\n");
        exit(EXIT_FAILURE);
    }

    // #### #### #### #### set kernel arguments #### #### #### #### ####

    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_values);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot set kernel argument for buffer_values\n");
        exit(EXIT_FAILURE);
    }
    error = clSetKernelArg(kernel, 1, sizeof(float), &diffusion_const);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot set kernel argument for diffusion_const\n");
        exit(EXIT_FAILURE);
    }

    //execute opencl kernel
    const size_t global[] = {width-2, height-2};
    const size_t global_offset[] = {1, 1};
    for (size_t i = 0; i < iterations; i++) {
        /*
        error = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL,
            (const size_t *) &global, NULL, 0, NULL, NULL);
            */
        error = clEnqueueNDRangeKernel(command_queue, kernel, 2, global_offset,
            (const size_t *) &global, NULL, 0, NULL, NULL);
        if (error != CL_SUCCESS) {
            fprintf(stderr, "cannot execute kernel\n");
            exit(EXIT_FAILURE);
        }
        //check status when debugging
        //status = clFlush(command_queue);
        //status = clFinish(command_queue);
        //printf("Status after kernel finishes: %d\n", status);
    }

    //read buffer from GPU back into memory (heat_values)
    error = clEnqueueReadBuffer(command_queue, buffer_values, CL_TRUE, 0,
          sizeof(cl_double) * nmb_points_padding, heat_values, 0, NULL, NULL);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot read results from buffer (GPU) back into memory\n");
        exit(EXIT_FAILURE);
    }
    //print out the result values
    //for (int h = 0; h < height; ++h) {
    //   for (int w = 0; w < width; ++w) {
    //        printf("%g ", heat_values[h * width + w]);
    //    }
    //    printf("\n");
    //}

    //block until all queued OpenCL commands are finished
    status = clFinish(command_queue);

    /*
    printf("Final values:\n");
    for (size_t i = 1; i < height-1; i++) {
        for (size_t j = 1; j < width-1; j++) {
            printf("%f ", heat_values[i * width + j]);
        }
        printf("\n");
    }
    */

    avg = 0;
    for (size_t i = 0; i < nmb_points_padding; ++i)
        avg += heat_values[i];
    avg /= nmb_points;
    printf("average: %g\n", avg);

    avg_diff = 0;
    double diff;
    for (size_t i = 1; i < height-1; i++) {
        for (size_t j = 1; j < width-1; j++) {
            diff = heat_values[i * width + j] - avg;
            avg_diff += diff < 0 ? (-diff) : diff;
        }
    }
    avg_diff /= nmb_points;
    printf("average absolute difference: %g\n", avg_diff);


    clReleaseMemObject(buffer_values);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    free(kernel_code);
    free(heat_values);

    exit(EXIT_SUCCESS);
}

//--    methods              //////////////////////////////////////////////////

void parseArguments(int argc, char *argv[], char *progname, unsigned int *iterations, float *diffusion_const)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s -n<iterations> -d<diffusion constant>\n", progname);
        exit(EXIT_FAILURE);
    }

    int opt;
    char *endptr;
    while ((opt = getopt(argc, argv, "n:d:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            *iterations = convertToInt(optarg);
            break;
        case 'd':
            *diffusion_const = strtod(optarg, &endptr);
            if (endptr == optarg)
            {
                fprintf(stderr, "No digits where found for the diffusion constant!\n");
                exit(EXIT_FAILURE);
            }
            /* If we got here, strtod() successfully parsed a number */
            if (*endptr != '\0')
            { /* In principle not necessarily an error... */
                fprintf(stderr, "Attention: further characters after number: %s\n", endptr);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Usage: %s -n<iterations> -d<diffusion constant>\n", progname);

            exit(EXIT_FAILURE);
        }
    }
    //check for validity of arguments:
    if (*iterations < 0)
    {
        fprintf(stderr, "Error: Number of iterations is invalid/missing!\n");
        exit(EXIT_FAILURE);
    }
    if (*diffusion_const <= 0) {
        fprintf(stderr, "Error: Diffusion constant is invalid/missing!\n");
        exit(EXIT_FAILURE);
    }
}

long convertToInt(char *arg)
{
    char *endptr;
    long number;

    //10 = decimal system, endptr is to check if strtol gave us a number
    number = strtol(arg, &endptr, 10);

    if ((errno == ERANGE && (number == LONG_MAX || number == LONG_MIN)) || (errno != 0 && number == 0))
    {
        fprintf(stderr, "Failed to convert input to number!\n");
        exit(EXIT_FAILURE);
    }
    if (endptr == arg)
    {
        fprintf(stderr, "No digits where found!\n");
        exit(EXIT_FAILURE);
    }

    /* If we got here, strtol() successfully parsed a number */
    if (*endptr != '\0')
    { /* In principle not necessarily an error... */
        fprintf(stderr, "Attention: further characters after number: %s\n", endptr);
        exit(EXIT_FAILURE);
    }
    return number;
}
