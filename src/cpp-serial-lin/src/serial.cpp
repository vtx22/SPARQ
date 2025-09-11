#include "serial.hpp"
#include <errno.h>
#include <string.h>

Serial::Serial()
{
    set_timeouts(50, 50, 0, 50, 0);
}

Serial::~Serial()
{
    close();
}

/*
Get baud rate constant for termios
@param baud Baud rate as integer
@return Returns speed_t constant
*/
speed_t Serial::get_baud_constant(uint32_t baud)
{
    switch (baud)
    {
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return B115200;
    }
}

/*
Open a serial port
@param port Port Name, e.g. "/dev/ttyUSB0" or "/dev/ttyACM0"
@param baud Baud Rate
@return Returns 0 when successful
*/
int Serial::open(const char *port, uint32_t baud)
{
    if (_is_open)
    {
        close();
    }

    _port = port;
    _baud = baud;

    _fd = ::open(port, O_RDWR | O_NOCTTY | O_SYNC);

    if (_fd < 0)
    {
        if (errno == ENOENT || errno == ENOTDIR)
        {
            return SERIAL_ERR::PORT_NOT_FOUND;
        }
        return SERIAL_ERR::OPEN_ERR;
    }

    _is_open = true;

    int err = update_com_state();
    if (err != SERIAL_ERR::OK)
    {
        return err;
    }

    err = update_timeouts();
    if (err != SERIAL_ERR::OK)
    {
        return err;
    }

    return SERIAL_ERR::OK;
}

/*
Close the serial port if open
@return Returns 0 when successful
*/
int Serial::close()
{
    _is_open = false;

    if (_fd >= 0)
    {
        return (::close(_fd) == 0 ? SERIAL_ERR::OK : SERIAL_ERR::CLOSE_ERR);
    }

    return SERIAL_ERR::OK;
}

/*
Send bytes of vector via the open serial port
@param data Data vector that contains the bytes for sending
@return Returns number of bytes written or error code
*/
int Serial::write(const std::vector<uint8_t> &data)
{
    return write(data.data(), data.size());
}

/*
Send bytes of array via the open serial port
@param data Data byte array
@param length Number of bytes to send
@return Returns number of bytes sent or error code
*/
int Serial::write(const uint8_t *data, int length)
{
    if (!_is_open)
    {
        return SERIAL_ERR::NOT_OPEN;
    }

    ssize_t bytes_written = ::write(_fd, data, length);

    if (bytes_written < 0)
    {
        close();
        return SERIAL_ERR::WRITE_ERR;
    }

    return bytes_written;
}

/*
Send a string message
@param message String to send
@return Returns number of bytes sent or error code
*/
int Serial::print(std::string message)
{
    return write((uint8_t *)message.c_str(), message.size());
}

/*
Send int as string
@param value Decimal value to print
@return Returns number of bytes sent or error code
*/
int Serial::print(int value)
{
    return print(std::to_string(value));
}

/*
Send float as string
@param value Float value to print
@return Returns number of bytes sent or error code
*/
int Serial::print(float value)
{
    return print(std::to_string(value));
}

/*
Send float as string with variable precision
@param value Float value to print
@param precision Number of decimal places
@return Returns number of bytes sent or error code
*/
int Serial::print(float value, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;

    return print(ss.str());
}

/*
Read bytes to buffer array
@param buffer Data byte array for buffering the data
@param length Number of bytes to read
@return Returns number of bytes read or error code
*/
int Serial::read(uint8_t *buffer, int length)
{
    if (!_is_open)
    {
        return SERIAL_ERR::NOT_OPEN;
    }

    ssize_t bytes_read = ::read(_fd, buffer, length);

    if (bytes_read < 0)
    {
        close();
        return SERIAL_ERR::READ_ERR;
    }

    return bytes_read;
}

/*
Applies the saved communication settings to the open port
@return Returns 0 when successful
*/
int Serial::update_com_state()
{
    if (!_is_open)
    {
        return SERIAL_ERR::NOT_OPEN;
    }

    if (tcgetattr(_fd, &_tty) != 0)
    {
        return SERIAL_ERR::GET_COM_STATE;
    }

    // Set baud rate
    speed_t baud_const = get_baud_constant(_baud);
    cfsetispeed(&_tty, baud_const);
    cfsetospeed(&_tty, baud_const);

    // Clear existing flags
    _tty.c_cflag &= ~(CSIZE | PARENB | PARODD | CSTOPB | CRTSCTS);
    _tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    _tty.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL);
    _tty.c_oflag &= ~OPOST;

    // Set byte size
    switch (_byte_size)
    {
    case 5:
        _tty.c_cflag |= CS5;
        break;
    case 6:
        _tty.c_cflag |= CS6;
        break;
    case 7:
        _tty.c_cflag |= CS7;
        break;
    case 8:
        _tty.c_cflag |= CS8;
        break;
    default:
        _tty.c_cflag |= CS8;
        break;
    }

    // Set parity (Windows: 0=None, 1=Odd, 2=Even, 3=Mark, 4=Space)
    switch (_parity)
    {
    case 0: // No parity
        break;
    case 1: // Odd parity
        _tty.c_cflag |= (PARENB | PARODD);
        break;
    case 2: // Even parity
        _tty.c_cflag |= PARENB;
        break;
    case 3: // Mark parity (not directly supported, use odd as fallback)
        _tty.c_cflag |= (PARENB | PARODD);
        break;
    case 4: // Space parity (not directly supported, use even as fallback)
        _tty.c_cflag |= PARENB;
        break;
    }

    // Set stop bits (Windows: 0=1bit, 1=1.5bits, 2=2bits)
    if (_stop_bits == 2)
    {
        _tty.c_cflag |= CSTOPB; // 2 stop bits
    }
    // Note: 1.5 stop bits not supported in Linux, default to 1 stop bit

    // Enable receiver and set local mode
    _tty.c_cflag |= (CLOCAL | CREAD);

    if (tcsetattr(_fd, TCSANOW, &_tty) != 0)
    {
        return SERIAL_ERR::SET_COM_STATE;
    }

    return SERIAL_ERR::OK;
}

/*
Applies the saved timeout settings to the open port
@return Returns 0 when successful
*/
int Serial::update_timeouts()
{
    if (!_is_open)
    {
        return SERIAL_ERR::NOT_OPEN;
    }

    // Convert Windows timeout behavior to Linux VMIN/VTIME
    // This is a simplified mapping - Windows timeouts are more complex

    if (_rd_interval > 0)
    {
        _tty.c_cc[VMIN] = 0;
        _tty.c_cc[VTIME] = (_rd_interval + 99) / 100; // Convert ms to deciseconds (0.1s)
    }
    else
    {
        _tty.c_cc[VMIN] = 1;
        _tty.c_cc[VTIME] = (_rd_total + 99) / 100; // Convert ms to deciseconds
    }

    if (tcsetattr(_fd, TCSANOW, &_tty) != 0)
    {
        return SERIAL_ERR::SET_TIMEOUTS;
    }

    return SERIAL_ERR::OK;
}

/*
Update the communication settings. If the port is not open, settings are saved and applied on open() call
@param baud Baud Rate
@param byte_size Number of bits per byte, allowed are: 4, 5, 6, 7 or 8
@param stop_bits Stop bits, where 0 = 1 bit, 1 = 1.5 bits and 2 = 2 bits
@param parity Parity Bit, where 0 = None, 1 = Odd, 2 = Even, 3 = Mark and 4 = Space
@return Returns 0 when successful
*/
int Serial::set_comm_state(uint32_t baud, uint8_t byte_size, uint8_t stop_bits, uint8_t parity)
{
    // Ditch forbidden settings
    if (parity > 4 || stop_bits > 2 || byte_size < 5 || byte_size > 8)
    {
        return SERIAL_ERR::INVALID_PARAM;
    }

    _baud = baud;
    _byte_size = byte_size;
    _stop_bits = stop_bits;
    _parity = parity;

    int err = update_com_state();

    if (err == SERIAL_ERR::NOT_OPEN)
    {
        return SERIAL_ERR::OK;
    }

    return err;
}

/*
Set the read and write timeouts. If the port is not open, settings are saved and applied on open() call
@param rd_interval Maximum time between read chars
@param rd_total Constant in milliseconds
@param rd_mult Multiplier of characters
@param wr_total Constant in milliseconds
@param wr_mult Multiplier of characters
@return Returns 0 when successful
*/
int Serial::set_timeouts(uint32_t rd_interval, uint32_t rd_total, uint32_t rd_mult, uint32_t wr_total, uint32_t wr_mult)
{
    _rd_interval = rd_interval;
    _rd_total = rd_total;
    _rd_mult = rd_mult;
    _wr_total = wr_total;
    _wr_mult = wr_mult;

    int err = update_timeouts();

    if (err == SERIAL_ERR::NOT_OPEN)
    {
        return SERIAL_ERR::OK;
    }

    return err;
}

/*
Set the baud rate. If the port is not open, baud rate is saved and applied on open() call
@return Returns 0 when successful
*/
int Serial::set_baud_rate(uint32_t baud)
{
    _baud = baud;

    int err = update_com_state();

    if (err == SERIAL_ERR::NOT_OPEN)
    {
        return SERIAL_ERR::OK;
    }

    return err;
}

/*
Get the last know state of the port (open/closed)
@return Returns true if the port was opened, false if the port is closed
*/
bool Serial::get_open()
{
    return _is_open;
}

/*
Get the ids of all available serial ports
@return Returns a vector that contains all port ids, e.g. "/dev/ttyUSB0" -> 0
*/
std::vector<uint8_t> Serial::get_port_ids()
{
    std::vector<uint8_t> port_ids;

    // Check common serial port patterns
    std::vector<std::string> patterns = {
        "/dev/ttyUSB", "/dev/ttyACM", "/dev/ttyS", "/dev/ttyAMA"};

    for (const auto &pattern : patterns)
    {
        for (uint8_t i = 0; i < 255; i++)
        {
            std::string port_path = pattern + std::to_string(i);

            // Try to open the port to see if it exists
            int test_fd = ::open(port_path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
            if (test_fd >= 0)
            {
                ::close(test_fd);
                port_ids.push_back(i);
            }
        }
    }

    return port_ids;
}

/*
Get the names of all available serial ports
@return Returns a string vector that contains all port names "/dev/ttyXXXn"
*/
std::vector<std::string> Serial::get_port_names()
{
    return get_port_names(true);
}

/*
Get the names of all available serial ports
@param add_prefix If true, "/dev/tty" prefix is added to the port identifier
@return Returns a string vector that contains all port names with or without prefix
*/
std::vector<std::string> Serial::get_port_names(bool add_prefix)
{
    std::vector<std::string> port_names;

    // Check common serial port patterns
    std::vector<std::string> patterns = {
        "/dev/ttyUSB", "/dev/ttyACM", "/dev/ttyS", "/dev/ttyAMA"};

    for (const auto &pattern : patterns)
    {
        for (uint8_t i = 0; i < 255; i++)
        {
            std::string port_path = pattern + std::to_string(i);

            // Try to open the port to see if it exists
            int test_fd = ::open(port_path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
            if (test_fd >= 0)
            {
                ::close(test_fd);

                if (add_prefix)
                {
                    port_names.push_back(port_path);
                }
                else
                {
                    // Extract just the device part
                    std::string device_part = port_path.substr(port_path.find_last_of('/') + 1);
                    port_names.push_back(device_part);
                }
            }
        }
    }

    return port_names;
}