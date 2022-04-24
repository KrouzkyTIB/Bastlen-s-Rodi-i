#ifndef __DISPLAY_DRIVER_H__
#define __DISPLAY_DRIVER_H__

#include "../Time/time.h"

void initDisplay();
void showTime(Time actualTime);
void handleDotsBlink(Time actualTime, uint8_t mode);


#endif