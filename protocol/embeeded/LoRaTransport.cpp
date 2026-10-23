#include "LoRaTransport.h"

#if defined(ARDUINO_ARCH_ESP32)

volatile bool LoRaTransport::_received_frame = false;

LoRaTransport::LoRaTransport() :radio(new Module(NSS, DIO1, RST, BUSY)),
                                display(128,64,&Wire, OLED_RST)
{    
}

void IRAM_ATTR LoRaTransport:: onReceive()
{
    _received_frame = true;
}

int16_t LoRaTransport::begin_rad() // also ini the OLED! 
{
    this->oled_init();
    //What will be the parameters?
    /*
    SF7-SF12: higher SF= each symbol is stretched longer = more robust ->+ range-> slower
    */

    int state =  radio.begin();
    if(state == RADIOLIB_ERR_NONE)
    {
        //I do the settings? -> radio.setFrequency() -> radio.setBandwith()...?
        radio.setDio1Action(onReceive);
        radio.startReceive();
    }
    return state;
}



void LoRaTransport::oled_init()
{
    // 1. Ligar Vext (alimentação do OLED)
    pinMode(VEXT_CTRL, OUTPUT);
    digitalWrite(VEXT_CTRL, LOW);   // activo baixo — liga
    delay(50);

    // 2. Reset do OLED
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, HIGH);
    delay(10);
    digitalWrite(OLED_RST, LOW);
    delay(10);
    digitalWrite(OLED_RST, HIGH);
    delay(10);

    // 3. Iniciar I2C nos pinos certos
    Wire.begin(OLED_SDA, OLED_SCL);

    // 4. Iniciar o display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("Falha OLED");
        return;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    oled_print("LoRa Transport\nBoot OK");
}

void LoRaTransport::oled_print(const char* msg, ...)
{
    char buf[128];

    va_list args;
    va_start(args,msg);
    vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    // Wrap manual: 21 chars por linha, 8 linhas de altura
    const int MAX_CHARS_PER_LINE = 21;
    int line = 0;
    int col = 0;

    for (int i = 0; buf[i] != '\0' && line < 8; i++) {
        if (buf[i] == '\n' || col >= MAX_CHARS_PER_LINE) {
            line++;
            col = 0;
            display.setCursor(0, line * 8);
            if (buf[i] == '\n') continue;
        }
        display.write(buf[i]);
        col++;
    }

    display.display();
}

void LoRaTransport::oled_log(const char* msg, ...)
{
    char buf[128];
    va_list args;
    va_start(args, msg);
    vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    const int MAX_LINES = 8;   // 64px tall / 8px per line

    // Screen full? wipe and start from the top again.
    if (log_line >= MAX_LINES) {
        display.clearDisplay();
        log_line = 0;
    }

    // On the very first line after a clear, make sure the buffer is blank.
    if (log_line == 0) {
        display.clearDisplay();
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, log_line * 8);
    display.print(buf);       // one log entry = one line

    log_line++;
    display.display();
}


void LoRaTransport::updateFrame() // puts chip in earing mode -> waiting for frames 
{
    if(!_received_frame) return;
    _received_frame = false;

    uint8_t rx_buffer[32];
    size_t len = 0;

    int state = radio.readData(rx_buffer, 32);
    if( state != RADIOLIB_ERR_NONE )
    {
        //for debug purpose print the error to the Display!
        return;
    }
    //need to build the frame!...
    len = radio.getPacketLength();
    for(int i = 0; i< len; i++)
    {
        this->parser.processIncomingByte(rx_buffer[i]);//this already does all the job! frame inside parser.Frame!
    }
}

int16_t LoRaTransport::sendFrame(const Frame &f)//Acts as TX
{
    //I need to pass this frame into a buffer -> radio.transmit(buffer)!
    //What's my Max Size of Frame? -> 19 bytes
    //Create 32 spaces in memomy to transfer the frame data to! 
    uint8_t tx_buffer[32]; //what's the buffer type? char/unint8_t ?! - both 1 byte
    uint8_t index = 0; 
    

    //first we pass the header that's always a fixed size
    tx_buffer[index++] = f.sof;
    memcpy(&tx_buffer[index],&f.header,4); 
    index+=4; 

    if( f.header.len > MAX_PAYLOAD)
        return -1; // should be maybe a neg value! 
        
    //Payload    
    memcpy(&tx_buffer[index],f.payload.raw,f.header.len); // copy payload content!
    index += f.header.len;

    uint16_t outCrc = parser.calculateCRC16((const uint8_t*)&f.header, 4 + f.header.len);
    memcpy(&tx_buffer[index], &outCrc, 2);
    index += 2;

    int state = radio.transmit(tx_buffer,index); // no need o compute CRC! 
    if(state != RADIOLIB_ERR_NONE)
    {
        //return state;
        return state;
    }
    radio.startReceive();//after sending we start listening! 

    return index; //ends up being the number of bytes we sent!
}

bool LoRaTransport::hasNewFrame() const
{
    return this->parser.newFrameReady;
}

void LoRaTransport::getFrame(Frame& outFrame)
{
    this->parser.newFrameReady = false;
    memcpy(&outFrame,&parser.frame,sizeof(Frame));
}

#endif // ARDUINO_ARCH_ESP32