#include "CircularBuffer.h"
#include <string.h>



CircularBuffer::CircularBuffer(): write(0), read(0), count(0)
{}

bool CircularBuffer::push(const Frame &f)
{
    //check if it's empy! 
    if(isFull())
    {
        //I don't save the frame! ---- FOR NOW! WILL BE IN OUR INTEREST TO IMPLEMENT SOMETHING DIF!
        return false;//We are not re-writing data! 
    }
    else{
        //buffer[write] = f;
        memcpy(&buffer[write], &f, sizeof(Frame));
        write = (write + 1) % MAX_SIZE;
        count++;
        return true;
    }
}

bool CircularBuffer::pop(Frame &f)
{
    if(isEmpty()) return false;
    //f = buffer[read];
    memcpy(&f, &buffer[read], sizeof(Frame));
    read = (read+1) % MAX_SIZE;
    count--;
    return true;
}


bool CircularBuffer:: isEmpty()
{
    if(count == 0) return true;
    else return false;
}

bool CircularBuffer:: isFull()
{
    if(count == MAX_SIZE) return true;
    else return false;
}