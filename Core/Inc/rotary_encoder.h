#ifndef INC_ROTARY_ENCODER_H_
#define INC_ROTARY_ENCODER_H_

#include "main.h"
#include <stdint.h>

// #define __CLASSIC_DEBOUNCE
// #define __FULL_CYCLE_DEBOUNCE
#define DOUBLE_TIME_ROTARY_ENCODER_DEBOUNCE

// Debounce for switch
#define ROT_ENCOD_SW_DEBOUNCE_DELAY 10
// Debounce for rotary encoder
#define ROT_ENCOD_DEBOUNCE_DELAY 10

void scanRotaryEncoder(uint16_t GPIO_Pin);
void scanRotaryEncoderSwitch(void);

#endif /* INC_ROTARY_ENCODER_H_ */
