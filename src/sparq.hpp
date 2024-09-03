#ifndef SPARQ_HPP
#define SPARQ_HPP

#include <vector>

struct sparq_data_point
{
    float x;
    float y;

} typedef sparq_data_point;

struct sparq_dataset
{
    uint8_t uuid;
    std::vector<float> x_values;
    std::vector<float> y_values;

} typedef sparq_dataset;

std::vector<sparq_dataset> data;

#endif // SPARQ_HPP