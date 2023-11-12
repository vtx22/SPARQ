#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <windows.h>
#include <vector>
#include <stdint.h>
#include <string>
#include <iostream>

class Serial
{
public:
   Serial();
   ~Serial();

   int open(const char *port, int baud);
   int close();

   static std::vector<uint8_t> get_port_ids();
   static std::vector<std::string> get_port_names();

   int set_write_timeout(uint16_t ms);
   int set_read_timeout(uint16_t ms);

   size_t read(uint8_t *buffer, int n);
   size_t read_line();

   size_t write(std::vector<uint8_t> data);
   size_t write(uint8_t *data, int length);

   bool is_open = false;

private:
   const char *_port;
   int _baud;

   HANDLE _handle;
};

#endif // SERIAL_HPP