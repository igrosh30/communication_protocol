#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>


// 1. Tell the compiler to pack structs tightly without padding!
#pragma pack(push, 1)

const uint8_t SOF_VALUE     = 0x7E;
const uint8_t MAX_PAYLOAD   = 12; 

/*

SOF = 0x7E

HEADER (4 bytes):
  SRC  : 1 byte (node id)
  DEST : 1 byte (node id)
  TYPE : 1 byte (command)
  LEN  : 1 byte (payload size)

PAYLOAD:
  LEN bytes

CRC:
  2 bytes (CRC16 of HEADER+PAYLOAD)
  */

//note that this will need to be only 1byte! 
enum NodeId 
{
    HOST = 0,
    TANK_MCU = 1,
};

enum MsgType
{
    PING        = 0,
    PONG        = 1,
    CMD         = 10,    //ACTUATION.
    DATA_SEND   = 20,    //DATA - sending or asking? 
    DATA_ASK    = 21,      //we can ask the tank state! 
    ACK         = 100, 
    MSG_ERROR   = 99,
};

struct Header
{
    uint8_t src;
    uint8_t dest;
    uint8_t type;
    uint8_t len;
};

union PayloadData//Size of a union is always the size of it's biggest member!
{
    uint8_t raw[MAX_PAYLOAD];

    struct {
        uint8_t cmdId;      //Byte 1   : corresponding enum
        uint8_t param[MAX_PAYLOAD-1];     //Bytes 1-11 : data after cmdId
    }data;

    struct { 
        uint8_t cmdId;     // Which command are we acknowledging?
        uint8_t status;    // 0 = OK, 1 = Error, 2 = Busy, 3 = Timeout
    } action_ack;
    
    // Named views for common replies (same memory!)
    // struct {  uint8_t cmdId; uint8_t battery_V;} battery_reply; 
    // struct { uint16_t depth_mm; } pop_up_params;
};

enum CMD_ID : uint8_t
{
    WAKE_UP         =1,
    START_CYCLE     =2,
    SET_TIMEOUT     =3,
    ASK_TANK_STATE  =10,
    REPORT_STATUS   =11,
    TANK_MOVING     =12,
    TANK_READY      =19,
};


#pragma pack(pop)
#endif
