#ifndef __TIME__HPP__
#define __TIME__HPP__
#include <Arduino.h>
#include <DS3231.h>

#define HOURS_IN_DAY 24
#define MINUTES_IN_HOUR 60

struct Time{
    byte hours;
    byte mins;
    byte seconds;
};

Time getTime();
void initTime(uint8_t hours, uint8_t mins, uint8_t seconds);
void prepareSettingsTime();
void incrementHour();
void decrementHour();
void incrementMinute();
void decrementMinute();
Time getSettingsTime();
void setTime(Time time);

#endif