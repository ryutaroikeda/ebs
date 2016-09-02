#include "linear_algebra.h"
#include <stdio.h>

void compute_feature_vector(const float x, struct vector* result) {
  result->values[0] = 1.0;
  float power_of_x = x;
  for (size_t dimension_index = 1; dimension_index < MAX_VECTOR_LENGTH;
      dimension_index++) {
    result->values[dimension_index] = power_of_x;
    power_of_x *= x;
  }
}

void compute_data(const float* measurements, const float* observations,
    const size_t data_length, struct data* data) {
  for (size_t data_index = 0; data_index < data_length; data_index++) {
    compute_feature_vector(measurements[data_index],
        &data[data_index].features);
    data[data_index].observation = observations[data_index];
  }
}

float compute_dot_product(const struct vector* v, const struct vector* w) {
  float dot_product = 0;
  for (size_t dimension_index = 0; dimension_index < MAX_VECTOR_LENGTH;
      dimension_index++) {
    dot_product += v->values[dimension_index] * w->values[dimension_index];
  }
  return dot_product;
}

float compute_error(const struct data* data, const struct vector* model,
    const size_t data_length) {
  float error = 0.0;
  for (size_t data_index = 0; data_index < data_length; data_index++) {
    float prediction = compute_dot_product(&data[data_index].features, model);
    error += (prediction - data[data_index].observation) *
			(prediction - data[data_index].observation);
  }
  error *= 1.0 / (2.0 * data_length);
  return error;
}

int main(const int argc, const char** argv) {
  (void) argc;
  (void) argv;
	struct vector model;
	float mock_model[MAX_VECTOR_LENGTH] = {1, 0, 0, 0};
	for (size_t dimension_index = 0; dimension_index < 4; dimension_index++) {
		model.values[dimension_index] = mock_model[dimension_index];
	}
	float measurement = 2.0;
	struct vector features;
	compute_feature_vector(measurement, &features);
	
	float prediction = compute_dot_product(&features, &model);
	printf("predict: %f\n", prediction);

	float measurements[] = {1, 1.1};
	float observations[] = {1, 2};
	size_t data_length = sizeof(measurements) / sizeof(measurements[0]);

	struct data data;
	compute_data(measurements, observations, data_length, &data);

	float error = compute_error(&data, &model, data_length);
	printf("model error: %f\n", error);

  return 0;
}
