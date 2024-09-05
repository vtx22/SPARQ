#include "DataHandler.hpp"

DataHandler::DataHandler(Serial *sp) : _sp(sp)
{
}

DataHandler::~DataHandler()
{
}

void DataHandler::update()
{
}

sparq_message_t DataHandler::receive_message()
{
}