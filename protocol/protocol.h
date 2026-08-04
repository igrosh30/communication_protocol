#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>


// 1. Tell the compiler to pack structs tightly without padding!
#pragma pack(push, 1)

const uint8_t SOF_VALUE     = 0x7E;
const uint8_t MAX_PAYLOAD   = 12; 

/*
Rasp5 config:
ssh control: 
username: pi5crasuser
password: #CRasfeUP2026
hostname: pi5-cras 
//Start the ven: source venv/bin/activate
//deactivate: 

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
    BASE = 1,
    BUOY = 2,
    EXT  = 3
};

enum MsgType
{
    WAKE_UP     = 0,    //start all the COM'S
    GO_SLEEP    = 99,
    PING        = 1,
    PONG        = 2,
    DATA        = 3,    //SEND & ASK - direction in payload specifies 
    ACK         = 4     //Processed correctly your message!
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
        uint8_t direction;  //Byte 0   : 0-> SEND / 1-> ASK/CMD
        uint8_t cmdId;      //Byte 1   : corresponding enum
        uint8_t param[MAX_PAYLOAD-2];     //Byte 2-7 : bytes for data
    }data;

    struct { 
        uint8_t cmdId;     // Which command are we acknowledging?
        uint8_t status;    // 0 = OK, 1 = Error, 2 = Busy, 3 = Timeout
    } action_ack;
    
    // Named views for common replies (same memory!)
    struct {  uint8_t cmdId; uint8_t battery_V;} battery_reply; 
    struct { uint16_t depth_mm; } pop_up_params;
};

enum CMD_ID : uint8_t
{
    ASK_BATTERY =1,
    POP_UP      =2,
    RETREIVE    =3,
    STOP_MOTOR  =4,
    SET_DEPTH   =5,// set depth from HOST
    BUOY_AT_BASE=6,
};

enum AckStatus : uint8_t
{
    ACK_OK        = 0,
    ACK_GPS_ERR   = 1,
    ACK_LORA_ERR  = 11,
    ACK_ERROR     = 2,
    ACK_BUSY      = 3,
    ACK_TIMEOUT   = 4,
    ACK_TIMEOUT_PING_BUOY = 5
};


#pragma pack(pop)
#endif
