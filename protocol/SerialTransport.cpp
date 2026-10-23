#include <string.h>
#include "SerialTransport.h"


// Default constructor - does nothing
SerialTransport::SerialTransport() 
    : serialPort(nullptr), 
      parser() 
{
 
}

void SerialTransport::init(Stream* port, bool useAcousticModem)
{
    if(port != nullptr)
    {
        this->isModem = useAcousticModem;
        this->serialPort = port;
        this->parser.init();
    }
}

int16_t SerialTransport::sendFrame(const Frame& f) 
{    
    uint8_t bytes_written = 0;
    uint8_t txBuffer[32]; 
    uint8_t index = 0;

    txBuffer[index++] = f.sof;  

    memcpy(&txBuffer[index], &f.header, 4);
    index += 4;

    if (f.header.len > 0) {
        memcpy(&txBuffer[index], f.payload.raw, f.header.len);
        index += f.header.len;
    }

    //CRC calculation to validade the information on the other side!
    uint16_t outCrc = parser.calculateCRC16((const uint8_t*)&f.header, 4 + f.header.len);
    memcpy(&txBuffer[index], &outCrc, 2);
    index += 2;

    if(this->isModem)
    {
        //dain the modem buffer!
        uint32_t lastByteTime = millis();
        while (millis() - lastByteTime < 50)
        {
            if (serialPort->available()) {
                serialPort->read();  // drain
                lastByteTime = millis();
            }
        }
        serialPort->print("$B");

        //send leght of the message -> index! 
        char lenStr[4];
        sprintf(lenStr, "%02d", index);
        serialPort->print(lenStr);
    }
    bytes_written = serialPort->write(txBuffer, index);

    if (this->isModem) {
        serialPort->print("\r\n");//end the message! 
    }

    serialPort->flush();
    return bytes_written;
}

void SerialTransport::updateFrame() {//Read bytes available
    uint8_t bytesCount = 0;
    while(serialPort->available() && bytesCount < MAX_BYTES_PER_CYCLE) //16 bytes Serial.available() && 
    {
        uint8_t incomingByte = (uint8_t)serialPort->read();   
        //DBG("RX 0x%02X\n", incomingByte);
        parser.processIncomingByte(incomingByte);//Update the State!
        bytesCount++;
    }
}

bool SerialTransport::hasNewFrame() const 
{
    return parser.newFrameReady;
}

void SerialTransport::getFrame(Frame& outFrame)
{
    parser.newFrameReady = false;//IMPORTANT to RESET the flag! we got the frame 
    memcpy(&outFrame,&parser.frame, sizeof(Frame));
}

void SerialTransport::reset()
{
    parser.init();
}
 