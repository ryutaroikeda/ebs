#ifndef _ebs_monte_carlo_h_
#define _ebs_monte_carlo_h_

#include <stddef.h>

void
simulate(const double*, const size_t, const double*, const size_t, double*,
		const size_t);

double
compute_mean(const double*, const size_t);

double
compute_variance(const double*, const size_t);

#endif
