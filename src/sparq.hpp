#ifndef SPARQ_HPP
#define SPARQ_HPP

#include <vector>
#include <cstring>

struct sparq_data_point
{
    float x;
    float y;

} typedef sparq_data_point;

struct sparq_dataset
{
    uint8_t id;
    std::string name = "";
    std::vector<float> x_values;
    std::vector<float> y_values;

} typedef sparq_dataset;

#endif // SPARQ_HPP