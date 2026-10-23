#ifndef SERIALTRANSPORT_H
#define SERIALTRANSPORT_H

#include <Arduino.h> 
#include "ITransport.h"

class SerialTransport: public ITransport{
private:
    
    Stream* serialPort; // object used to send Data!
    bool isModem;

public:
    FrameParser parser;
    const int MAX_BYTES_PER_CYCLE = 32;

    SerialTransport();

    void init(Stream* port, bool useAcousticModem = false);
    
    void updateFrame() override;
    void getFrame(Frame& outFrame) override;
    bool hasNewFrame() const override;
    int16_t sendFrame(const Frame& f) override;
    void reset();
};

#endif  // DATALINK_H