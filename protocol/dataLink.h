#ifndef DATALINK_H
#define DATALINK_H

#include "protocol.h"
#include "debug.h"
#include "FrameParser.h"

#ifdef ARDUINO

class DataLink {
private:
    Stream* serialPort;
    bool isModem;

public:
    FrameParser parser;
    const int MAX_BYTES_PER_CYCLE = 32;

    DataLink();
    void init(Stream* port, bool useAcousticModem = false);
    uint8_t sendFrame(const Frame& f);
    void update();
    bool hasNewFrame() const;
    void getFrame(Frame& outFrame);
    void reset();
};

#endif  // ARDUINO
#endif  // DATALINK_H