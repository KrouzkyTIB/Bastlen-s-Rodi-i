#include "display.hpp"

#include <Arduino.h>
/**
 * @brief Indexy jednotlivých číslic v číslicových displayích, které jsou umístěny na desce
 * 
 */
enum digits {
    HOURS_FIRST_DIGIT,
    HOURS_SECOND_DIGIT,
    MINUTES_FIRST_DIGIT,
    MINUTES_SECOND_DIGIT
};
/**
 * @brief Piny pro jednotlivé číslice 
 * 
 */
uint8_t digitsPins[] = {9, 10, 5, 6};
/**
 * @brief Zapnuté bity pro jednotlivé číslice pro jejich zobrazení na displayi
 * 
 */
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

/**
 * @brief Znak pro vypnutý alarm při jeho nastavování
 * 
 */
uint8_t alarmTurnedOffSign[] = {0, 0, 0, 0, 0, 0, 1}; // -
/**
 * @brief Inicializuje display hodin
 * 
 */
void initDisplay() {
    pinMode(RCLK, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(SER, OUTPUT);
    pinMode(DOTS_PIN, OUTPUT);
    for (uint8_t i = 0; i < NUMBER_OF_DIGITS; i++) {
        pinMode(digitsPins[i], OUTPUT);
    }
}
/**
 * @brief Vypne všechny číslice
 * 
 */
void turnOffAllDigits() {
    for (uint8_t i = 0; i < NUMBER_OF_DIGITS; i++) {
        digitalWrite(digitsPins[i], LOW);
    }
}
/**
 * @brief Posune bit do registru SN74HC595, který udržuje informace o tom, co se má ukázat na displayi
 * 
 * @param value 
 */
void shiftBit(uint8_t value) {
    digitalWrite(SRCLK, LOW);
    digitalWrite(SER, value);
    digitalWrite(SRCLK, HIGH);
}

/**
 * @brief Zobrazí požadované číslo od 0 do 9 na požadovaném číslicovém displayi od 0 do 3
 * 
 * @param number Číslo, které chceme zobrazit
 * @param digit Index displaye, který chceme použít pro zobrazení čísla
 */
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

/**
 * @brief Zobrazí námi požadovaný čas na displayi
 * 
 * @param hours Hodina, kterou chceme zobrazit
 * @param minutes Minuta, kterou chceme zobrazit
 * @param digitDelay Délka svitu jedné číslice
 */
void showTime(uint8_t hours, uint8_t minutes, uint16_t digitDelay) {
    showHours(hours, digitDelay);
    showMinutes(minutes, digitDelay);
    turnOffAllDigits();
}

/**
 * @brief Funkce pro blikání s prostředními led diodami, zde slouží pro ukázání každé sudé sekundy
 * 
 * @param seconds aktuální sekundy
 */
void blinkWithDots(uint8_t seconds) {
    digitalWrite(DOTS_PIN, seconds % 2 == 0);
}

/**
 * @brief Okamžitě vypne rozsvícenou dvojtečku
 * 
 */
void turnOffDots() {
    digitalWrite(DOTS_PIN, LOW);
}
/**
 * @brief Zobrazí hodinu předanou jako parametr hours na prvních 2 číslicích displaye
 * 
 * @param hours Hodina, která se zobrazí na prvních 2 displayích
 * @param digitDelay Délka zobrazení jednotlivých číslic
 */
void showHours(uint8_t hours, uint16_t digitDelay) {
    uint8_t hoursFirstDigit = hours / 10;
    if (hoursFirstDigit != 0) {
        showNumber(hoursFirstDigit, HOURS_FIRST_DIGIT);
        delay(digitDelay);
    }
    showNumber(hours % 10, HOURS_SECOND_DIGIT);
    delay(digitDelay);
}
/**
 * @brief Zobrazí minuty předané jako parametr minutes na posledních 2 číslicích displaye
 * 
 * @param minutes Minuty, které se zobrazí na posledních 2 číslicích displaye
 * @param digitDelay Délka zobrazení jednotlivých číslic
 */

void showMinutes(uint8_t minutes, uint16_t digitDelay) {
    uint8_t minutesFirstDigit = minutes / 10;
    showNumber(minutesFirstDigit, MINUTES_FIRST_DIGIT);
    delay(digitDelay);
    showNumber(minutes % 10, MINUTES_SECOND_DIGIT);
    delay(digitDelay);
}
/**
 * @brief Zobrazí blikajicí hodiny pro mód nastavení času
 * 
 * @param currentTime Čas, který je právě uložen v Arduinu
 * @param settingsTime Čas, který nastaví uživatel
 */
void showBlinkingHours(Time currentTime, Time settingsTime) {
    if (currentTime.seconds % 2 == 0) {
        showHours(settingsTime.hours, DIGIT_DELAY);
    } else {
        turnOffAllDigits();
        delay(DIGIT_DELAY * 2);
    }
    showMinutes(settingsTime.mins, DIGIT_DELAY);
}

/**
 * @brief Zobrazí blikající minuty pro mód nastavení času
 * 
 * @param currentTime Čas, který je právě uložen v Arduinu
 * @param settingsTime Čas, který nastaví uživatel
 */

void showBlinkingMinutes(Time currentTime, Time settingsTime) {
    showHours(settingsTime.hours, DIGIT_DELAY);
    if (currentTime.seconds % 2 == 0) {
        showMinutes(settingsTime.mins, DIGIT_DELAY);
    } else {
        turnOffAllDigits();
        delay(DIGIT_DELAY * 2);
    }
}

/**
 * @brief Zobrazí pomlčku na display
 * 
 * @param digit Číslo displaye kam checeme zobrazit pomlčku
 */

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

/**
 * @brief Zobrazí blikající pomlčky po dvojicích podle toho, zdali chceme, aby blikaly na pozici hodin a nebo na pozici minut
 * 
 * @param currentTime Aktuální čas nastavený na arduino
 * @param isHourPosition Logická hodnota, zdali mají pomlčky blikat na pozici hodin nebo na pozici minut
 */

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
