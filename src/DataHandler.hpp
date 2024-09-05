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

    void update();
    sparq_message_t receive_message();

private:
    Serial *_sp;
};

#endif // DATA_HANDLER_HPP