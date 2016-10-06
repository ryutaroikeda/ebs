#ifndef _ebs_monte_carlo_h_
#define _ebs_monte_carlo_h_

#include <stddef.h>

/* Simulates the given tasks and put the results in simulated_times. 
 * simulated_times_length is the number of simulations to run. */
void simulate(const double*, size_t, const double*, size_t, double*, size_t);

/* Compute the mean. */
double compute_mean(const double*, const size_t);

/* Compute the variance. */
double compute_variance(const double*, const size_t);

#endif
