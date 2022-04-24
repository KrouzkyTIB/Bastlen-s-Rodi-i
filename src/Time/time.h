#ifndef __TIME_H__
#define __TIME_H__

#include <Arduino.h>

typedef struct Time{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
}Time;

#endif