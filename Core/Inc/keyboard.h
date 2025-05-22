#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_
#include "main.h"

#define KEYPAD_ROWS 8
#define KEYPAD_COLS 8
#define KEYS_STATE_BUFFER_SIZE 5

typedef struct {
  uint8_t new[KEYS_STATE_BUFFER_SIZE];
  uint8_t newNb;
  uint8_t already[KEYS_STATE_BUFFER_SIZE];
  uint8_t alreadyNb;
} KeysState_t;

typedef struct {
  uint8_t prevPressed;
  uint8_t pressed;
} Key_t;

typedef struct {
  uint16_t gpio;
  GPIO_TypeDef* port;
} RowColGpio_t;

KeysState_t getKeysState();

#endif /* INC_KEYBOARD_H_ */
