#ifndef HOST_FRAME_PARSER_H
#define HOST_FRAME_PARSER_H

#include "protocol.h"
#include  "debug.h"

enum RxState : uint8_t
{
    STATE_WAIT_FOR_SOF,
    STATE_READ_HEADER,
    STATE_READ_PAYLOAD,
    STATE_READ_CRC
};

//--------PARSER LOGIC-------------// What is a Frame ?!
struct Frame 
{
    uint8_t sof;                    // Always 0x7E
    Header header;                  // Your 4-byte header
    PayloadData payload;            // Maximum allowed payload
    uint16_t crc;                   // 2-byte error check
};

class FrameParser //reception tool for each Station
{
private:

public:
    RxState currentState;
    uint8_t countBytesH;//count header bytes to process
    uint8_t countBytesP;//count payload bytes to process
    uint8_t countBytesCRC;
    Frame frame;
    bool newFrameReady;
    
    void init();
    //void printDebug();

    void processIncomingByte(uint8_t byte);
    uint16_t calculateCRC16(const uint8_t* data, uint8_t len);

};

#endif
