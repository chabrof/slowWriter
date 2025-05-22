#include "keyboard.h"
#include <stdio.h>

const uint8_t keymap[KEYPAD_ROWS][KEYPAD_COLS] = {
  {13, 12, 11, 10, 9, 8, 7, 6 },
  {5, 4, 3, 2, 1, 0 , 14, 15 },
  {16, 17, 18, 19, 20, 21, 22, 23 },
  {24, 25, 26, 27, 40, 39, 38, 37 },
  {36, 35, 34, 33, 32, 31, 30, 29 },
  {28, 41, 42, 43, 44, 45, 46, 47 },
  {48, 49, 50, 51, 52, 60, 59, 58 },
  {57, 56, 55, 54, 53, 61, 62, 63 }
};


const RowColGpio_t cols[] = {
 { KEYB_COL_OUT_0_Pin, KEYB_COL_OUT_0_GPIO_Port },
 { KEYB_COL_OUT_1_Pin, KEYB_COL_OUT_1_GPIO_Port },
 { KEYB_COL_OUT_2_Pin, KEYB_COL_OUT_2_GPIO_Port },
 { KEYB_COL_OUT_3_Pin, KEYB_COL_OUT_3_GPIO_Port },
 { KEYB_COL_OUT_4_Pin, KEYB_COL_OUT_4_GPIO_Port },
 { KEYB_COL_OUT_5_Pin, KEYB_COL_OUT_5_GPIO_Port },
 { KEYB_COL_OUT_6_Pin, KEYB_COL_OUT_6_GPIO_Port },
 { KEYB_COL_OUT_7_Pin, KEYB_COL_OUT_7_GPIO_Port },
};

const RowColGpio_t rows[] = {
 { KEYB_ROW_IN_0_Pin, KEYB_ROW_IN_0_GPIO_Port },
 { KEYB_ROW_IN_1_Pin, KEYB_ROW_IN_1_GPIO_Port },
 { KEYB_ROW_IN_2_Pin, KEYB_ROW_IN_2_GPIO_Port },
 { KEYB_ROW_IN_3_Pin, KEYB_ROW_IN_3_GPIO_Port },
 { KEYB_ROW_IN_4_Pin, KEYB_ROW_IN_4_GPIO_Port },
 { KEYB_ROW_IN_5_Pin, KEYB_ROW_IN_5_GPIO_Port },
 { KEYB_ROW_IN_6_Pin, KEYB_ROW_IN_6_GPIO_Port },
 { KEYB_ROW_IN_7_Pin, KEYB_ROW_IN_7_GPIO_Port },
};

Key_t keypad[KEYPAD_ROWS][KEYPAD_COLS];

void RawScanKeypad(uint8_t raw[KEYPAD_ROWS][KEYPAD_COLS]) {
  for (uint8_t row = 0; row < KEYPAD_ROWS; row++)
    for (uint8_t col = 0; col < KEYPAD_COLS; col++)
      raw[row][col] = 0;

  for (uint8_t colIdx = 0; colIdx < KEYPAD_COLS; colIdx++) {
    // Met toutes les colonnes à 0
    for (uint8_t colIdxLoc = 0; colIdxLoc < KEYPAD_COLS; colIdxLoc++) {
      HAL_GPIO_WritePin(cols[colIdxLoc].port, cols[colIdxLoc].gpio, GPIO_PIN_RESET);
    }

    // Active une seule colonne
    HAL_GPIO_WritePin(cols[colIdx].port, cols[colIdx].gpio, GPIO_PIN_SET);

    HAL_Delay(1); // stabilisation

    for (uint8_t rowIdx = 0; rowIdx < KEYPAD_ROWS; rowIdx++) {
      GPIO_PinState state;
      state = HAL_GPIO_ReadPin(rows[rowIdx].port, rows[rowIdx].gpio);

      if (state == GPIO_PIN_SET) {
        printf("Raw scan: %i %i\r\n", rowIdx, colIdx);
        raw[rowIdx][colIdx] = 1;
      }
    }
  }
}

KeysState_t getKeysState(void) {
  uint8_t raw[KEYPAD_ROWS][KEYPAD_COLS];
  RawScanKeypad(raw);
  KeysState_t keysState = {.alreadyNb = 0, .newNb = 0};

  for (uint8_t rowIdx = 0; rowIdx < KEYPAD_ROWS; rowIdx++) {
    for (uint8_t colIdx = 0; colIdx < KEYPAD_COLS; colIdx++) {
      Key_t keypadLoc = {.prevPressed = 0, .pressed = 0}; 
      uint8_t val = raw[rowIdx][colIdx];

      if (val == 1) {
        // Pressed !
        uint8_t code = keymap[rowIdx][colIdx];

        if (keypad[rowIdx][colIdx].prevPressed == 1) {
          // also Pressed before !
          if (keysState.alreadyNb < KEYS_STATE_BUFFER_SIZE) {
            keysState.already[keysState.alreadyNb++] = code;
          }
        } else {
          if (keysState.newNb < KEYS_STATE_BUFFER_SIZE) {
            keysState.new[keysState.newNb++] = code;
          }
        }
      }
      keypadLoc.pressed = val;
      keypadLoc.prevPressed = val;
      keypad[rowIdx][colIdx] = keypadLoc;
    }
  }
  return keysState;
}
