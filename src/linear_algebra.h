#ifndef _ebs_linear_algebra_h_
#define _ebs_linear_algebra_h_

#include <stdlib.h>

#define MAX_VECTOR_LENGTH 4

struct vector {
  float values[MAX_VECTOR_LENGTH];
};

struct data {
  struct vector features;
  float observation;
};

void compute_feature_vector(const float, struct vector*);

void compute_data(const float*, const float*, const size_t, struct data*);

float compute_dot_product(const struct vector*, const struct vector*);

float compute_error(const struct data*, const struct vector*, const size_t);

#endif
