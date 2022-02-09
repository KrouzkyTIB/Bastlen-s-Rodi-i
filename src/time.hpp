#ifndef __TIME__HPP__
#define __TIME__HPP__
#include <Arduino.h>
#include <DS3231.h>

#define HOURS_IN_DAY 24
#define MINUTES_IN_HOUR 60
#define ALARM_PIN 7


struct Time{
    byte hours;
    byte mins;
    byte seconds;
};

struct AlarmSettings{
    Time ringTime;
    bool on;
};

Time getTime();
void initTime(uint8_t hours, uint8_t mins, uint8_t seconds);
void prepareSettingsTime(bool forAlarmSetting);
void incrementHour();
void decrementHour();
void incrementMinute();
void decrementMinute();
Time getSettingsTime();
void setTime(Time time);
AlarmSettings getAlarmSettings();
void setAlarmTime(Time time);
void toggleAlarmStatus();
void initAlarmSettings();
void checkAlarm(Time currentTime);
void turnOffAlarm();

#endif