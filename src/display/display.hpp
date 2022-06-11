#ifndef __DISPLAY__HPP__
#define __DISPLAY__HPP__
#include <Arduino.h>

#include "time/time.hpp"

#define SER 2
#define RCLK 3
#define SRCLK 4
#define NUMBER_OF_DIGITS 4
#define NUMBER_OF_NUMBERS 10
#define NUMBER_OF_SEGMENTS 7
#define DOTS_PIN 8
#define DIGIT_DELAY 1

extern uint8_t digitsPins[NUMBER_OF_DIGITS];
extern uint8_t numbers[NUMBER_OF_NUMBERS][NUMBER_OF_SEGMENTS];

void initDisplay();
void showNumber(uint8_t number, uint8_t digit);
void showTime(uint8_t hours, uint8_t minutes, uint16_t digitDelay);
void blinkWithDots(uint8_t seconds);
void turnOffDots();
void showMinutes(uint8_t minutes, uint16_t digitDelay);
void showHours(uint8_t hours, uint16_t digitDelay);
void turnOffAllDigits();
void showBlinkingHours(Time currentTime, Time settingsTime);
void showBlinkingMinutes(Time currentTime, Time settingsTime);
void showBlinkingDashes(Time currentTime, bool isHourPosition);
#endif
