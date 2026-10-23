#ifndef LORATRANSPORT_H
#define LORATRANSPORT_H

#if defined(ARDUINO_ARCH_ESP32)

#include "ITransport.h"
#include <RadioLib.h>

#define NSS 8
#define RST 12 
#define BUSY 13 
#define DIO1 14//flags many events

//OLED display:
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <cstdarg>

#define OLED_SDA    17
#define OLED_SCL    18
#define OLED_RST    21
#define VEXT_CTRL   36


//static -> makes it belong to the class and not to the object instanses! 

class LoRaTransport: public ITransport
{
    private:
        

        SX1262 radio;
        Frame f;
        FrameParser parser;
        static volatile bool _received_frame; // can we use this also? -> better to
        static void IRAM_ATTR onReceive();

        Adafruit_SSD1306 display;
        void oled_init();
        int log_line = 0;  

    public:

        LoRaTransport();        
        int16_t begin_rad();
        void oled_print(const char* msg,...);
        void oled_log(const char* msg, ...);
        void updateFrame() override;
        void getFrame(Frame& outFrame) override;
        bool hasNewFrame() const override;
        int16_t sendFrame(const Frame& f) override;

};

#endif // ARDUINO_ARCH_ESP32

#endif // LORATRANSPORT_H