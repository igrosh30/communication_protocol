#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "FrameParser.h"


#define MAX_SIZE 8

class CircularBuffer
{
public:
    /*
    I need to define:
        1- array
        2- pointer write/read
        3- count variable
        4- push()/pull() 
        5- isEmpty()/isFull()
    */
    
    Frame buffer[MAX_SIZE];//each frame has 16 bytes -> total 128 bytes
    
    //pointers to read/write
    uint8_t write;
    uint8_t read;
    uint8_t count;
    
    CircularBuffer();
    bool push(const Frame &f);
    bool pop(Frame &f);

    bool isEmpty();
    bool isFull();

};

#endif