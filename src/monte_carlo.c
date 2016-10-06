#include "monte_carlo.h"
#include "utility.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Return a simulated completion time for the given tasks. */
static double simulate_completion_time(const double*, const size_t, const
    double*, const size_t);

double simulate_completion_time(const double* velocities, const size_t
    velocities_length, const double* estimated_times, const size_t
    estimated_times_length) {
	assert(NULL != velocities);
	assert(NULL != estimated_times);

	double predicted_completion_time = 0.0;
	size_t estimated_times_index;
	for (estimated_times_index = 0;
			estimated_times_index < estimated_times_length;
			estimated_times_index++) {
		size_t random_velocities_index = (size_t) random_from_range(0,
        (int) velocities_length);

    // If we don't have data, use the estimate directly.
    if (0 == velocities_length) {
      predicted_completion_time += estimated_times[estimated_times_index];
      continue;
    }

		predicted_completion_time += estimated_times[estimated_times_index] /
			velocities[random_velocities_index];
	}

	return predicted_completion_time;
}

void simulate(const double* velocities, const size_t velocities_length,
		const double* estimated_times, const size_t estimated_times_length,
		double* simulated_times, size_t simulated_times_length) {
	assert(NULL != velocities);
	assert(NULL != estimated_times);
	assert(NULL != simulated_times);

	size_t simulated_times_index;
  for (simulated_times_index = 0; simulated_times_index <
      simulated_times_length; simulated_times_index++) {
    simulated_times[simulated_times_index] = simulate_completion_time(
        velocities, velocities_length, estimated_times,
        estimated_times_length);
	}
}

double compute_mean(const double* measurements, const size_t
    measurements_length) {
	assert(NULL != measurements);
	double mean = 0.0;

	size_t measurements_index;
	for (measurements_index = 0; measurements_index < measurements_length;
			measurements_index++) {
		mean += measurements[measurements_index];
	}

	mean /= (double) measurements_length;
	return mean;
}

double compute_variance(const double* const measurements,
		const size_t measurements_length) {
	assert(NULL != measurements);

	if (measurements_length < 2) {
		return 0.0;
	}

	const double mean = compute_mean(measurements, measurements_length);
  double variance = 0.0;
  size_t measurements_index;
	for (measurements_index = 0; measurements_index < measurements_length;
			measurements_index++) {
    double deviation_from_mean = measurements[measurements_index] - mean;
    variance += deviation_from_mean * deviation_from_mean;
  }

  variance *= 1.0 / (double) (measurements_length - 1);
  return variance;
}
