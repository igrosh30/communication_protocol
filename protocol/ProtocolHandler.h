#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include "FrameParser.h"
#include "ITransport.h"


class ProtocolHandler {

protected:
    NodeId myId;

public:
    // Constructor
    ProtocolHandler(NodeId id);

    uint8_t processFrame(const Frame& f, ITransport &ddl);//returns bytes writen
    
    uint8_t replyAck(const Frame& f_received, uint8_t status, ITransport &ddl);
    void buildReplyHeader(Frame& f, uint8_t type, uint8_t payloadLen, uint8_t dest);

    // --- THE HARDWARE ABSTRACTION LAYER (HAL) ---
    // The Parent forces the Child to write these three specific functions:
    virtual bool storeReceivedData(uint8_t cmdId, const uint8_t* param) = 0;
    virtual bool handleSpecificCommands(const Frame& f, Frame& reply) = 0; 
    virtual uint8_t readBatteryLevel() = 0;//returns %of battery
};

#endif
