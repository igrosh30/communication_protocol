#pragma once

#ifdef ARDUINO
  #include <Arduino.h>
  #ifdef DEBUG_ENABLED
    #define DBG(fmt, ...)   Serial.printf(fmt, ##__VA_ARGS__)
    #define DBGLN(msg)      Serial.println(msg)
  #else
    #define DBG(...)
    #define DBGLN(...)
  #endif
#else //linux base code:
  #include <cstdio>
  #ifdef DEBUG_ENABLED
    #define DBG(fmt, ...)   printf(fmt, ##__VA_ARGS__)
    #define DBGLN(msg)      puts(msg)
  #else
    #define DBG(...)
    #define DBGLN(...)
  #endif
#endif