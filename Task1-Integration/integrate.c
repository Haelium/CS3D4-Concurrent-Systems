#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define NUM_OF_THREADS 2

typedef struct integrate_io {
    double input_lower_limit;
    double input_step_size;
    int input_num_of_steps;    
    double output_area;
} integrate_io;

double y (double x) {
    // When integrated with limits from 0 to 1, the result should approximate pi
    return 4 / (1 + x*x);
}

void* pt_integrate (void* args) {
    double lower = ((integrate_io*)args)->input_lower_limit;
    double step_size = ((integrate_io*)args)->input_step_size;
    int max_steps = ((integrate_io*)args)->input_num_of_steps;
    
    double total_area = 0;
    for (int s = 0; s < max_steps; s++) {
        total_area += y(lower + s * step_size) * step_size;
    }
    //printf("\ntotal_area (within pthread) = %lf\n", total_area);

    ((integrate_io*)args)->output_area = total_area;
    pthread_exit(NULL);
}

double integrate_y (double upper_limit, double lower_limit, double step_size) {
    assert(upper_limit > lower_limit);
    integrate_io integrate_io_array[NUM_OF_THREADS];
    // Some rounding error below
    int total_steps = (int) ((upper_limit - lower_limit) / step_size);
    int steps_per_thread = total_steps / (int)NUM_OF_THREADS;
    int remainder_steps = total_steps % NUM_OF_THREADS;
    //printf("\nTotal steps = %d\n", total_steps);
    //printf("steps per thread = %d\n", steps_per_thread);
    //printf("remainder steps (last thread) = %d\n", remainder_steps);

    // Initialise input
    for (int t = 0; t < NUM_OF_THREADS; t++) {
        // This could cause overlap between area calculate in each thread
        integrate_io_array[t].input_lower_limit = lower_limit + t * step_size * steps_per_thread;
        integrate_io_array[t].input_step_size = step_size;
        integrate_io_array[t].input_num_of_steps = steps_per_thread;
    }
    // Add remaining steps (result of integer division) to last thread (this is not ideal)
    integrate_io_array[NUM_OF_THREADS - 1].input_num_of_steps += remainder_steps;
    

    int rc;
    pthread_t threads[NUM_OF_THREADS];
    for (int t = 0; t < NUM_OF_THREADS; t++) { 
		//printf("Creating thread %d\n",t); 
		rc = pthread_create(&threads[t], NULL, pt_integrate, (void *)(&integrate_io_array[t])); 
		if (rc) { 
			printf("ERROR return code from pthread_create(): %d\n",rc); 
			exit(-1); 
		} 
	}
    
    // wait for threads to exit 
	for(int t = 0; t < NUM_OF_THREADS; t++) { 
		pthread_join(threads[t], NULL); 
	}

    double total_area = 0;
    for (int t = 0; t < NUM_OF_THREADS; t++) {
        total_area += integrate_io_array[t].output_area;
    }

    return total_area;
}

int main (int argc, char* argv[]) {
    // Set up timer
    struct timespec start_time, end_time;

    // get current time
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    double total_area = integrate_y(1, 0, 0.0000000005);
    //printf("Approximate integral of 4 / (1 + x^2) from %lf to %lf in steps of %lf is:\n", 0.0, 1.0, 0.0000000005);
    //printf("%1.25lf\n", total_area);
    
    // Get end time
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    printf("Time taken: %lu.%lu s\n", (end_time.tv_sec - start_time.tv_sec), (end_time.tv_nsec - start_time.tv_nsec));

    return 0;
}