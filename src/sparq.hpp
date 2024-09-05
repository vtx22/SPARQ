#ifndef SPARQ_HPP
#define SPARQ_HPP

#include <vector>
#include <string>
#include <cstdint>

struct sparq_data_point
{
    float x;
    float y;

} typedef sparq_data_point;

struct sparq_dataset
{
    uint8_t id = 0;
    std::string name = "";
    std::vector<float> x_values;
    std::vector<float> y_values;

} typedef sparq_dataset;

struct sparq_message_t
{
    uint8_t signature;
    uint8_t control;
    uint8_t nval;
    std::vector<uint8_t> ids;
    std::vector<float> values;
    uint8_t checksum;

} typedef sparq_message_t;

#endif // SPARQ_HPP