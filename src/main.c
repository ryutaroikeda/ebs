#include "monte_carlo.h"
#include <math.h>
#include <stdio.h>

#define MAX_SIMULATIONS 100
int
main(int argc, char** argv) {
	(void) argc;
	(void) argv;

	double velocities[] = {
		0.1, 0.2, .4, .2, .4, .3
	};

	size_t length = sizeof(velocities) / sizeof(velocities[0]);

	double estimates [] = {
		12, .5, 1
	};

	size_t estimates_length = sizeof(estimates) / sizeof(estimates[0]);

	double simulations[MAX_SIMULATIONS];

	simulate(velocities, length, estimates, estimates_length,
			simulations, MAX_SIMULATIONS);

	size_t i;
	for (i = 0; i < MAX_SIMULATIONS; i++) {
		printf("%f\n", simulations[i]);
	}

	double mean = compute_mean(simulations, MAX_SIMULATIONS);
	double variance = compute_variance(simulations, MAX_SIMULATIONS);
	double standard_deviation = sqrt(variance);

	printf("mean: %f\n", mean);
	printf("variance: %f\n", variance);
	printf("standard deviation: %f\n", standard_deviation);

	return 0;
}
