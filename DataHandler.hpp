#ifndef DATA_HANDLER_HPP
#define DATA_HANDLER_HPP

#include <cstdint>

#include "sparq.hpp"
#include "serial.hpp"

class DataHandler
{
public:
    DataHandler(Serial *sp);
    ~DataHandler();

    sparq_message_t receive_package();

private:
    Serial *_sp;
};

#endif // DATA_HANDLER_HPP