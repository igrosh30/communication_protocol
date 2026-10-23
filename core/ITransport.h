#ifndef ITRANSPORT_H
#define ITRANSPORT_H

#include "protocol.h"
#include "FrameParser.h"
#include "debug.h"

//THIS IS THE CONTRACT that a new transmission hardware implements! 

class ITransport
{
public:
    virtual void updateFrame() = 0;
    virtual void getFrame(Frame& outFrame) = 0;
    virtual bool hasNewFrame() const = 0;
    virtual int16_t sendFrame(const Frame &f) = 0;
};
#endif