#ifndef APP_LAYER_H
#define APP_LAYER_H

#include "FrameParser.h"


class AppLayer {

protected:
    NodeId myId;

public:
    // Constructor
    AppLayer(NodeId id);

    void processFrame(const Frame& f);
    void sendErrorAck(uint8_t cmdID, uint8_t dest);
    void buildReplyHeader(Frame& f, uint8_t type, uint8_t payloadLen, uint8_t dest);

    // --- THE HARDWARE ABSTRACTION LAYER (HAL) ---
    // The Parent forces the Child to write these three specific functions:
    virtual bool storeReceivedData(uint8_t cmdId, const uint8_t* param) = 0;
    virtual bool handleSpecificCommands(const Frame& f, Frame& reply) = 0; 
    virtual uint8_t readBatteryLevel() = 0;//returns %of battery
    virtual uint8_t transmitReply(const Frame& reply) = 0; //call the dtLink
};

#endif
