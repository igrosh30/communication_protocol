#pragma once
#include "ITransport.h"
#include <termios.h>

//Written for Linux, e.g. Jetson Nano, Rasp, etc. 

class LinuxSerialTransport: public ITransport
{

private:
    int serial_port;
    FrameParser parser;
    const int MAX_BYTES_PER_CYCLE = 32;

public:
    
    LinuxSerialTransport();//constructor
    ~LinuxSerialTransport();//de-constructor

    uint8_t init(const char* port, speed_t baudRate);
    /*
    1- error openning port
    2- error initializing tcgetattr
    3- error saving settings
    */

    void shutdown();

    void updateFrame() override;
    void getFrame(Frame& outFrame) override;
    bool hasNewFrame() const override;
    int16_t sendFrame(const Frame& f) override;

};