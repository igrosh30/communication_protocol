#include "LinuxSerialTransport.hpp"

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close(

#include <string.h>

/*
To write to a serial port, you write to the file
To read from a serial port, you read from the file.

struct termios {
  tcflag_t c_iflag;     input mode flags 
  tcflag_t c_oflag;     output mode flags 
  tcflag_t c_cflag;     control mode flags
  tcflag_t c_lflag;     local mode flags 
  cc_t c_line;          line discipline 
  cc_t c_cc[NCCS];     control characters 
};

VMIN = 0, VTIME = 0 - read() does not block, and will 
return immediately with what data is available. 
read() could return no bytes, or 1 or more bytes of data depending what 
was waiting in the buffer -> this is a pooling approach - what we want!
*/

LinuxSerialTransport::LinuxSerialTransport()
    : serial_port(-1), parser()
{
}

LinuxSerialTransport::~LinuxSerialTransport(){shutdown();}

uint8_t LinuxSerialTransport::init(const char* port, speed_t baudRate)
{
    // 1. Open the serial port for COM 
    this->serial_port = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
    if(this->serial_port < 0) 
    {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return 1;
    }

    
    // 2. Get current terminal attributes into a struct termios
    struct termios tty;
    //initialise before modify vals
    if(tcgetattr(serial_port, &tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        shutdown();
        return 2;
    }
    //whtiout this kernel treats the serial port like a human typing on a terminal!
    cfmakeraw(&tty);

    //now we can change the atributes of the port as needed!
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetspeed(&tty, baudRate); 

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        shutdown();
        return 3;
    }
    // 7. Init the parser
    parser.init();

    return 0;
}

void LinuxSerialTransport::shutdown()
{
    if (serial_port >= 0)
    {
        close(this->serial_port);
        this->serial_port = -1;
    }
}

void LinuxSerialTransport::updateFrame()
{
    // Same logic as your SerialTransport::updateFrame()
    // but replace serialPort->available() / serialPort->read()
    // with: read(fd, &byte, 1) — returns 0 or -1 when nothing available
    uint8_t bytesCount = 0;
    uint8_t byte;
    while(read(this->serial_port,&byte,1) == 1 && bytesCount<MAX_BYTES_PER_CYCLE)
    {
        parser.processIncomingByte(byte);
        bytesCount++;
    }
       
    // read up to MAX_BYTES_PER_CYCLE bytes in one call
    // ssize_t n = ???
    
    // feed each byte to parser.processIncomingByte()
}

bool LinuxSerialTransport::hasNewFrame() const
{
    return parser.newFrameReady;
}

void LinuxSerialTransport::getFrame(Frame& outFrame)
{
    parser.newFrameReady = false;
    memcpy(&outFrame, &parser.frame, sizeof(Frame));
}


int16_t LinuxSerialTransport::sendFrame(const Frame& f)
{
    uint8_t txBuffer[32];
    uint8_t index = 0;

    txBuffer[index++] = f.sof;

    memcpy(&txBuffer[index], &f.header, 4);
    index += 4;

    if (f.header.len > 0) {
        memcpy(&txBuffer[index], f.payload.raw, f.header.len);
        index += f.header.len;
    }

    uint16_t outCrc = parser.calculateCRC16((const uint8_t*)&f.header, 4 + f.header.len);
    memcpy(&txBuffer[index], &outCrc, 2);
    index += 2;

    ssize_t bytes_written = write(this->serial_port, txBuffer, index);
    tcdrain(this->serial_port);

    return (int16_t)bytes_written;
}
