#include "serial.hpp"

Serial::Serial()
{
}

Serial::~Serial()
{
   close();
}

int Serial::open(const char *port, int baud)
{
   _port = port;
   _baud = baud;
   std::string p = "\\\\.\\";
   p += _port;
   _handle = CreateFile(p.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

   if (_handle == INVALID_HANDLE_VALUE)
   {
      if (GetLastError() == ERROR_FILE_NOT_FOUND)
      {
         // Port not found
         return -1;
      }
      // Other error
      return -1;
   }

   // Do some basic settings
   DCB serialParams = {0};
   serialParams.DCBlength = sizeof(serialParams);

   GetCommState(_handle, &serialParams);
   serialParams.BaudRate = _baud;
   serialParams.ByteSize = 8;
   serialParams.StopBits = ONESTOPBIT;
   serialParams.Parity = NOPARITY;

   if (!SetCommState(_handle, &serialParams))
   {
      return -1;
   }

   // Set timeouts
   COMMTIMEOUTS timeout = {0};
   timeout.ReadIntervalTimeout = 50;
   timeout.ReadTotalTimeoutConstant = 50;
   timeout.ReadTotalTimeoutMultiplier = 50;
   timeout.WriteTotalTimeoutConstant = 50;
   timeout.WriteTotalTimeoutMultiplier = 10;

   if (!SetCommTimeouts(_handle, &timeout))
   {
      return -1;
   }

   is_open = true;

   return 0;
}

int Serial::close()
{
   is_open = false;
   return CloseHandle(_handle);
}

size_t Serial::write(std::vector<uint8_t> data)
{
   return write(&data[0], data.size());
}

size_t Serial::write(uint8_t *data, int length)
{
   DWORD bytes_written = 0;

   if (!WriteFile(_handle, (char *)data, length, &bytes_written, NULL))
   {
      return -1;
   }

   return bytes_written;
}

size_t Serial::read(uint8_t *buffer, int n)
{
   DWORD bytes_read = 0;

   if (!ReadFile(_handle, (char *)buffer, n, &bytes_read, NULL))
   {
      return -1;
   }

   return bytes_read;
}

std::vector<uint8_t> Serial::get_port_ids()
{
   char lpTargetPath[5000]; // buffer to store the path of the COMPORTS
   std::vector<uint8_t> port_ids;

   for (uint8_t i = 0; i < 255; i++)
   {
      std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
      DWORD port = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

      if (port != 0)
      {
         // std::cout << str << ": " << lpTargetPath << std::endl;
         port_ids.push_back(i);
      }

      if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
      }
   }

   return port_ids;
}

std::vector<std::string> Serial::get_port_names()
{
   std::vector<uint8_t> port_ids = get_port_ids();
   std::vector<std::string> coms;

   for (const auto &id : port_ids)
   {
      coms.push_back(std::string("COM") + std::to_string(id));
   }

   return coms;
}