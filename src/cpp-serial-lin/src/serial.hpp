#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <vector>
#include <stdint.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

/*
Serial Port Error Codes
*/
enum SERIAL_ERR
{
    OK = 0,               // Successful
    NOT_OPEN = -1,        // Port not open, no I/O possible
    READ_ERR = -2,        // Error while reading from port
    WRITE_ERR = -3,       // Error while writing to port
    SET_COM_STATE = -4,   // Cannot set Com State (baud, byte size, etc.)
    GET_COM_STATE = -5,   // Cannot get the current Com State from Handle
    SET_TIMEOUTS = -6,    // Cannot set the timeout parameters
    CLOSE_ERR = -7,       // Error while closing the port
    OPEN_ERR = -8,        // Error while opening the port
    INVALID_PARAM = -9,   // Given parameters are not allowed
    PORT_NOT_FOUND = -10, // Given port name not found
};

class Serial
{
public:
    Serial();

    ~Serial();

    int open(const char *port, uint32_t baud);
    int close();

    static std::vector<uint8_t> get_port_ids();
    static std::vector<std::string> get_port_names();
    static std::vector<std::string> get_port_names(bool add_prefix);

    int read(uint8_t *buffer, int n);

    int write(const std::vector<uint8_t> &data);
    int write(const uint8_t *data, int length);

    int print(std::string message);
    int print(int value);
    int print(float value);
    int print(float value, uint8_t precision);

    int set_comm_state(uint32_t baud, uint8_t byte_size, uint8_t stop_bits, uint8_t parity);
    int set_timeouts(uint32_t rd_interval, uint32_t rd_total, uint32_t rd_mult, uint32_t wr_total, uint32_t wr_mult);

    int set_baud_rate(uint32_t baud);

    bool get_open();

private:
    int update_com_state();
    int update_timeouts();
    speed_t get_baud_constant(uint32_t baud);

    const char *_port;

    bool _is_open = false;

    uint32_t _baud = 115200;
    uint8_t _byte_size = 8;
    uint8_t _stop_bits = 1; // 1 = one stop bit
    uint8_t _parity = 0;    // 0 = no parity

    int _fd; // file descriptor

    struct termios _tty;

    // Timeout parameters
    uint32_t _rd_interval = 50;
    uint32_t _rd_total = 50;
    uint32_t _rd_mult = 0;
    uint32_t _wr_total = 50;
    uint32_t _wr_mult = 0;
};

#endif // SERIAL_HPP