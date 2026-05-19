#include "AppLayer.h"
#include "debug.h"

AppLayer::AppLayer(NodeId id) {
    this->myId = id;
}

void AppLayer::processFrame(const Frame& f) {
    
    uint8_t type = f.header.type;
    uint8_t cmd  = f.payload.data.cmdId;
    DBG("[Base] RECEIVED frame from %d ",f.header.src);
    if (type == MsgType::PING)
    {
        Frame reply;
        this->buildReplyHeader(reply, PONG, 0, f.header.src);
        this->transmitReply(reply); 
        return;
    }
    else if(type == MsgType::DATA)
    {
        if(f.payload.data.direction == 0)//send Data!
        {
            bool sucess = storeReceivedData(cmd, f.payload.data.param);

            Frame ackFrame;
            this->buildReplyHeader(ackFrame, ACK, 2, f.header.src);
            //payoadRes:
            ackFrame.payload.action_ack.cmdId = cmd;
            ackFrame.payload.action_ack.status = sucess ? ACK_OK : ACK_ERROR;
            
            this->transmitReply(ackFrame);
            return;
        }
        else if(f.payload.data.direction == 1)//ASK/CMD!
        {
            if (cmd == ASK_BATTERY) 
            {
                Frame replyBattery;
                this->buildReplyHeader(replyBattery, DATA, 2, f.header.src);
                replyBattery.payload.battery_reply.cmdId = cmd; // We are sending it back
                replyBattery.payload.battery_reply.battery_V = this->readBatteryLevel();
                this->transmitReply(replyBattery);
                return;
            }
            else
            {//IF some ASKING or COMMAND is specific we call handleSpecificCommands! 
                //Handle specific repply!
                Frame customReply;
                if (handleSpecificCommands(f, customReply) == true) {
                    this->transmitReply(customReply);
                }
                else; //Trow error?
            }   
        }

        

    }
}
// Inside AppLayer.cpp
void AppLayer::sendErrorAck(uint8_t cmdID, uint8_t dest) {
    Frame reply;
    buildReplyHeader(reply, MsgType::ACK, 2, dest);
    reply.payload.action_ack.cmdId = cmdID;
    reply.payload.action_ack.status = ACK_ERROR; 
    transmitReply(reply);
}
void AppLayer::buildReplyHeader(Frame& f, uint8_t type, uint8_t payloadLen, uint8_t dest) {
    f.sof = SOF_VALUE;
    f.header.src = this->myId; // Automatically set to our ID
    f.header.dest = dest;
    f.header.type = type;
    f.header.len = payloadLen;
}
