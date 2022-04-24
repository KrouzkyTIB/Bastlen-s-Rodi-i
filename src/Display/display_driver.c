#include <Arduino.h>
#include "display_driver.h"

#define SER 2 // display IN
#define RCLK 3 // display show pin
#define SRCLK 4 // display clock pin
#define NUMBER_OF_DIGITS 4
#define NUMBER_OF_NUMBERS 10
#define NUMBER_OF_SEGMENTS 7
#define DOTS_PIN 8
#define DIGIT_DELAY 1


/*
    Zde jsme vytvořili structuru, která drží data pro jednotlivé displaye
    Prvek numberToShow nám říká jaké číslo na daný display chceme zobrazit
    Prvek turnOnDigit nám říká, jestli chceme daný display rozsvítit (třeba když jsou hodiny menší jak 10 tak nechceme ukazovat nulu jako první číslo)
*/

typedef struct {
    uint8_t numberToShow;
    bool turnOnDigit;
}DigitValue;


/*
    Zde jsme vytvořili enumeraci (vyjmenování prvků), která nám pomůže správně indexovat jednotlivé 7-segmentové displaye
*/
enum{
    HOUR_FIRST_DIGIT = 0,
    HOUR_SECOND_DIGIT,
    MINUTE_FIRST_DIGIT,
    MINUTE_SECOND_DIGIT
}DigitNames;

/*
    Enumerace módů na blikání teček na hodinách
*/

enum {
    BLINK_EACH_EVEN_SECOND, // Možnost nastevení blikání na každou sudou sekundu
    BLINK_EACH_ODD_SECOND // Možnost nastevení blikání na každou lichou sekundu
}DotsBlinkModes;


/*
 prvni 2 čísla jsou piny hodin, druhá 2 jsou piny minut
*/

const uint8_t DIGITS_PINS[] = {9, 10, 5, 6}; 


/*
    Dvou rozměrné pole ve které si uchováváme jaké segmenty máme rozsvít pro jednotlivá čísla
*/
const uint8_t NUMBERS[NUMBER_OF_NUMBERS][NUMBER_OF_SEGMENTS] = {
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

/*
Zde naincializujeme display a dvojtečku mezi disapayi
*/

void initDisplay(){
    pinMode(SER, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(DOTS_PIN, OUTPUT);
    for(uint8_t i = 0; i < NUMBER_OF_DIGITS; i++){
        pinMode(DIGITS_PINS[i], OUTPUT);
    }
}

/*
    Vloží hodnotu do posuvného registru a hodnotu posune
*/

void shiftBit(bool value) {
    digitalWrite(SRCLK, LOW);
    digitalWrite(SER, value);
    digitalWrite(SRCLK, HIGH);
}

/*
    Vypne všechny rozsvícené číslice
*/

void turnAllDigitsOff(){
    for (uint8_t i = 0; i < NUMBER_OF_DIGITS; i++) {
        digitalWrite(DIGITS_PINS[i], LOW);
    }

}

/*
    Zde zobrazíme číslici na display
    Postupujeme tak, že do 8 bitového posuvného registru postupně dáme hodnoty tak, aby po jejich zobrazení nám vyšla daná číslice
    Parametr digit nám říká na jaký display cheme dané číslo zobrazit
    Parametr number nám říká, co za číslo chceme zobrazit
*/


void showNumberOnDigit(uint8_t digit, uint8_t number){
    turnAllDigitsOff();
    digitalWrite(RCLK, LOW);
    shiftBit(0); // tečka u displaye chceme aby byla vypnuta
    for(uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++){
        shiftBit(NUMBERS[number][i]);
    }
    digitalWrite(RCLK, HIGH);
    digitalWrite(digit, HIGH);
}




/*
    zde ukazujeme čas na display
    Postupujeme tak, že si pro každou číslici na displayi připravíme co na ní ukážeme a zdali jí vůbec budeme zapínat
    Poté co budeme mít tyto data připravena tak jen projdeme pole a postupně rozsvítíme jednotlivé číslice
*/

void showTime(Time actualTime){
    DigitValue digitValues[NUMBER_OF_DIGITS];
    digitValues[HOUR_FIRST_DIGIT].numberToShow = actualTime.hours / 10;
    digitValues[HOUR_FIRST_DIGIT].turnOnDigit = digitValues[HOUR_FIRST_DIGIT].numberToShow != 0;
    digitValues[HOUR_SECOND_DIGIT].numberToShow = actualTime.hours % 10;
    digitValues[HOUR_SECOND_DIGIT].turnOnDigit = true;
    digitValues[MINUTE_FIRST_DIGIT].numberToShow = actualTime.minutes / 10;
    digitValues[MINUTE_FIRST_DIGIT].turnOnDigit = true;
    digitValues[MINUTE_SECOND_DIGIT].numberToShow = actualTime.minutes % 10;
    digitValues[MINUTE_SECOND_DIGIT].turnOnDigit = true;
    for (uint8_t i = 0; i < NUMBER_OF_DIGITS; i++){
        DigitValue actualValue = digitValues[i];
        if (actualValue.turnOnDigit){
            showNumberOnDigit(i, actualValue.numberToShow);
        }
        delay(DIGIT_DELAY);
    }
    turnAllDigitsOff();
}


/*
    Zde se staráme o blikání led diod mezi displayi
    Pokud je mód nastaven na hodnotu BLINK_EACH_ODD_SECOND ledky budou blikat každou lichou sekundu
    Pokud je mód nastaven na hodnotu BLINK_EACH_EVEN_SECOND ledky budou blikat každou sudou sekundu
    Jak je vidět ze zápisu příkazu digitalWrite tak jako druhý parametr je možné vložit podmínku, protože příkaz digitalWrite bere jako druhý parametr 0 nebo 1
*/

void handleDotsBlink(Time actualTime, uint8_t mode){
    if (mode == BLINK_EACH_ODD_SECOND){
        digitalWrite(DOTS_PIN, actualTime.seconds % 2 == 1);
    }else if(mode == BLINK_EACH_EVEN_SECOND){
        digitalWrite(DOTS_PIN, actualTime.seconds % 2 == 0);
    }
}










