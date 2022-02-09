#include "display.hpp"

#include <Arduino.h>
enum digits {
    HOURS_FIRST_DIGIT,
    HOURS_SECOND_DIGIT,
    MINUTES_FIRST_DIGIT,
    MINUTES_SECOND_DIGIT
};

uint8_t digitsPins[] = {9, 10, 5, 6};
uint8_t numbers[NUMBER_OF_NUMBERS][NUMBER_OF_SEGMENTS] = {
    {1, 1, 1, 1, 1, 1, 0},  // 0
    {0, 1, 1, 0, 0, 0, 0},  // 1
    {1, 1, 0, 1, 1, 0, 1},  // 2
    {1, 1, 1, 1, 0, 0, 1},  // 3
    {0, 1, 1, 0, 0, 1, 1},  // 4
    {1, 0, 1, 1, 0, 1, 1},  // 5
    {1, 0, 1, 1, 1, 1, 1},  // 6
    {1, 1, 1, 0, 0, 0, 0},  // 7
    {1, 1, 1, 1, 1, 1, 1},  // 8
    {1, 1, 1, 1, 0, 1, 1}   // 9
};

uint8_t alarmTurnedOffSign[] = {0, 0, 0, 0, 0, 0, 1};

void initDisplay() {
    pinMode(RCLK, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(SER, OUTPUT);
    pinMode(DOTS_PIN, OUTPUT);
    for (uint8_t i = 0; i < NUMBER_OF_DIGITS; i++) {
        pinMode(digitsPins[i], OUTPUT);
    }
}

void turnOffAllDigits() {
    for (uint8_t i = 0; i < NUMBER_OF_DIGITS; i++) {
        digitalWrite(digitsPins[i], LOW);
    }
}
void shiftBit(uint8_t value) {
    digitalWrite(SRCLK, LOW);
    digitalWrite(SER, value);
    digitalWrite(SRCLK, HIGH);
}

void showNumber(uint8_t number, uint8_t digit) {
    turnOffAllDigits();
    digitalWrite(RCLK, LOW);
    shiftBit(0);  // point should be turned off
    for (int i = NUMBER_OF_SEGMENTS - 1; i >= 0; i--) {
        shiftBit(numbers[number][i]);
    }
    digitalWrite(RCLK, HIGH);
    digitalWrite(digitsPins[digit], HIGH);
}

void showTime(uint8_t hours, uint8_t minutes, uint16_t digitDelay) {
    showHours(hours, digitDelay);
    showMinutes(minutes, digitDelay);
    turnOffAllDigits();
}

void blinkWithDots(uint8_t seconds) {
    digitalWrite(DOTS_PIN, seconds % 2 == 0);
}

void turnOffDots() {
    digitalWrite(DOTS_PIN, LOW);
}

void showHours(uint8_t hours, uint16_t digitDelay) {
    uint8_t hoursFirstDigit = hours / 10;
    if (hoursFirstDigit != 0) {
        showNumber(hoursFirstDigit, HOURS_FIRST_DIGIT);
        delay(digitDelay);
    }
    showNumber(hours % 10, HOURS_SECOND_DIGIT);
    delay(digitDelay);
}

void showMinutes(uint8_t minutes, uint16_t digitDelay) {
    uint8_t minutesFirstDigit = minutes / 10;
    showNumber(minutesFirstDigit, MINUTES_FIRST_DIGIT);
    delay(digitDelay);
    showNumber(minutes % 10, MINUTES_SECOND_DIGIT);
    delay(digitDelay);
}

void showBlinkingHours(Time currentTime, Time settingsTime) {
    if (currentTime.seconds % 2 == 0) {
        showHours(settingsTime.hours, DIGIT_DELAY);
    } else {
        turnOffAllDigits();
        delay(DIGIT_DELAY * 2);
    }
    showMinutes(settingsTime.mins, DIGIT_DELAY);
}
void showBlinkingMinutes(Time currentTime, Time settingsTime) {
    showHours(settingsTime.hours, DIGIT_DELAY);
    if (currentTime.seconds % 2 == 0) {
        showMinutes(settingsTime.mins, DIGIT_DELAY);
    } else {
        turnOffAllDigits();
        delay(DIGIT_DELAY * 2);
    }
}

void showDash(int digit) {
    turnOffAllDigits();
    digitalWrite(RCLK, LOW);
    shiftBit(0);  // point should be turned off
    for (int i = NUMBER_OF_SEGMENTS - 1; i >= 0; i--) {
        shiftBit(alarmTurnedOffSign[i]);
    }
    digitalWrite(RCLK, HIGH);
    digitalWrite(digitsPins[digit], HIGH);
}

void showBlinkingDashes(Time currentTime, bool isHourPosition) {
    if (isHourPosition) {
        if (currentTime.seconds % 2 == 0) {
            showDash(0);
            delay(DIGIT_DELAY);
            showDash(1);
            delay(DIGIT_DELAY);
        } else {
            turnOffAllDigits();
            delay(DIGIT_DELAY * 2);
        }
        showDash(2);
        delay(DIGIT_DELAY);
        showDash(3);
        delay(DIGIT_DELAY);
    } else {
        showDash(0);
        delay(DIGIT_DELAY);
        showDash(1);
        delay(DIGIT_DELAY);
        if (currentTime.seconds % 2 == 0) {
            showDash(2);
            delay(DIGIT_DELAY);
            showDash(3);
            delay(DIGIT_DELAY);
        } else {
            turnOffAllDigits();
            delay(DIGIT_DELAY * 2);
        }
    }
}
