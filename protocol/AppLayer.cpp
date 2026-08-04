#include "AppLayer.h"
#include "debug.h"

AppLayer::AppLayer(NodeId id) {
    this->myId = id;
}

uint8_t AppLayer:: processFrame(const Frame& f, ITransport &ddl) { // process Frame is called in Application Layer! Note: our AppLayer is not the AppLayer! is Network Layer! 
    
    uint8_t type = f.header.type;
    uint8_t cmd  = f.payload.data.cmdId;
    Frame reply;
    uint8_t bytes_writen;

    DBG("[BUOY] RECEIVED frame from %d\n ",f.header.src);
    if (type == MsgType::PING)
    {
        buildReplyHeader(reply,PONG,0,f.header.src);
    }
    else if(type == MsgType::DATA)
    {
        if(f.payload.data.direction == 0)//send Data!
        {
            bool sucess = storeReceivedData(cmd, f.payload.data.param);

            //Frame ackFrame;
            this->buildReplyHeader(reply, ACK, 2, f.header.src);
            //payoadRes:
            reply.payload.action_ack.cmdId = cmd;
            reply.payload.action_ack.status = sucess ? ACK_OK : ACK_ERROR;
            
            //this->transmitReply(ackFrame);
        }
        else if(f.payload.data.direction == 1)//ASK/CMD!
        {
            if (cmd == ASK_BATTERY) 
            {
                this->buildReplyHeader(reply, DATA, 2, f.header.src);
                reply.payload.battery_reply.cmdId = cmd; // We are sending it back
                reply.payload.battery_reply.battery_V = this->readBatteryLevel();
                //this->transmitReply(replyBattery);
                
            }
            else
            {//IF some ASKING or COMMAND is specific we call handleSpecificCommands! 
                //Handle specific repply!       
                if (handleSpecificCommands(f, reply) == true) {
                    //this->transmitReply(customReply);
                }
                else; //Trow error?
            }   
        }
    }
    bytes_writen = ddl.sendFrame(reply);
    
    DBG("[BUOY] Where writen %d bytes\n",bytes_writen);
    return bytes_writen;
}


// Inside AppLayer.cpp
uint8_t AppLayer::replyAck(const Frame& f_received, uint8_t status, ITransport &ddl) //this is sending or better build?
{
    //ONLY TYPE WITH DIFFERENT REPPLY IS PING! 
    Frame reply;
    buildReplyHeader(reply, MsgType::ACK, 2, f_received.header.dest);
    reply.payload.action_ack.cmdId = f_received.header.type;
    reply.payload.action_ack.status = status;  
    
    return ddl.sendFrame(reply);//Send back
}

void AppLayer::buildReplyHeader(Frame& f, uint8_t type, uint8_t payloadLen, uint8_t dest) {
    f.sof = SOF_VALUE;
    f.header.src = this->myId; // Automatically set to our ID
    f.header.dest = dest;
    f.header.type = type;
    f.header.len = payloadLen;
}
