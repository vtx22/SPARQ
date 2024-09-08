#pragma once

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

