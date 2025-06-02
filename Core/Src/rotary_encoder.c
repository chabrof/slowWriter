#include "rotary_encoder.h"
#include <stdint.h>
#include <string.h>
#include <stm32h723xx.h>
#include <stdio.h>

uint32_t rot_encod_left_lastTick = 0;
uint32_t rot_encod_right_lastTick = 0;
uint32_t rotary_encoder_left_cycle_tstp = 0; // 0: NOT_BEGAN positive int : BEGAN CYCLE tstp
uint32_t rotary_encoder_right_cycle_tstp = 0; // 0: NOT_BEGAN positive int : BEGAN CYCLE tstp
// Switch scan
uint32_t lastDebounceTime = 0;
GPIO_PinState lastButtonState = GPIO_PIN_SET;
GPIO_PinState buttonState = GPIO_PIN_SET;

#define ROTARY_LOG_SIZE 32
volatile uint8_t rotary_log[ROTARY_LOG_SIZE];
volatile uint8_t rotary_log_head = 0;
volatile uint8_t rotary_log_tail = 0;
// Buffer for rotary encoder events
#define ROTARY_L_END 1
#define ROTARY_R_END 2
#define ROTARY_L_BEGIN 3
#define ROTARY_R_BEGIN 4

#define ROTARY_L_LINE_FALL 3
#define ROTARY_R_LINE_FALL 4
#define ROTARY_BAD_L_END 5
#define ROTARY_BAD_R_END 6
#define ROTARY_BAD_L_BEGIN 7
#define ROTARY_BAD_R_BEGIN 8
#define ROTARY_L_CYCLE_OUT_OF_TIME 9
#define ROTARY_R_CYCLE_OUT_OF_TIME 10

#define ROTARY_LEFT_CYCLE_BEGIN 11
#define ROTARY_RIGHT_CYCLE_BEGIN 12
#define ROTARY_LEFT_CYCLE_1 13
#define ROTARY_RIGHT_CYCLE_1 14
#define ROTARY_LEFT_CYCLE_2 15
#define ROTARY_RIGHT_CYCLE_2 16
#define ROTARY_LEFT_CYCLE_END 17
#define ROTARY_RIGHT_CYCLE_END 18
#define ROTARY_LEFT_CYCLE_SHORT_END 19
#define ROTARY_RIGHT_CYCLE_SHORT_END 20

#define ROTARY_LEFT_CYCLE_ERROR 21
#define ROTARY_RIGHT_CYCLE_ERROR 22 
// Fonction à appeler dans l'ISR pour logger un événement
void rotary_log_event(uint8_t event)
{
    uint8_t next_head = (rotary_log_head + 1) % ROTARY_LOG_SIZE;
    if (next_head != rotary_log_tail) { // Buffer not full
        rotary_log[rotary_log_head] = event;
        rotary_log_head = next_head;
    }
    // Sinon, on ignore l'événement (overflow)
}

void rotary_process_log(void)
{
  while (rotary_log_tail != rotary_log_head) {
    uint8_t event = rotary_log[rotary_log_tail];
    rotary_log_tail = (rotary_log_tail + 1) % ROTARY_LOG_SIZE;
    switch(event) {
      case ROTARY_L_END:
        printf("==> L \r\n");
        break;
      case ROTARY_R_END:
        printf("==> R\r\n");
        break;
      case ROTARY_L_BEGIN:
        printf("L begin\r\n");
        break;
      case ROTARY_R_BEGIN:
        printf("R begin\r\n");
        break;
      case ROTARY_BAD_L_END:
        printf("L BAD\r\n");
        break;
      case ROTARY_BAD_R_END:
        printf("R BAD\r\n");
        break;
      case ROTARY_BAD_L_BEGIN:
        printf("L BEGIN BAD\r\n");
        break;
      case ROTARY_BAD_R_BEGIN:
        printf("R BEGIN BAD\r\n");
        break;
      case ROTARY_L_CYCLE_OUT_OF_TIME:
        printf("L OUT OF TIME\r\n\r\n");
        break;
      case ROTARY_R_CYCLE_OUT_OF_TIME:
        printf("R OUT OF TIME\r\n\r\n");
        break;
      case ROTARY_LEFT_CYCLE_BEGIN:
        printf("Left cycle begin\r\n");
        break;
      case ROTARY_RIGHT_CYCLE_BEGIN:
        printf("Right cycle begin\r\n");
        break;  
      case ROTARY_LEFT_CYCLE_1:
        printf("Left cycle 1\r\n");
        break;      
      case ROTARY_RIGHT_CYCLE_1:    
        printf("Right cycle 1\r\n");
        break;    
      case ROTARY_LEFT_CYCLE_2:
        printf("Left cycle 2\r\n");
        break;  
      case ROTARY_RIGHT_CYCLE_2:
        printf("Right cycle 2\r\n\r\n");
        break;    
      case ROTARY_LEFT_CYCLE_END:
        printf("         =>  Left ! \r\n\r\n");
        break;  
      case ROTARY_RIGHT_CYCLE_END:
        printf("         => Right !\r\n\r\n");
        break;  
      case ROTARY_LEFT_CYCLE_SHORT_END:
        printf("Left cycle short end\r\n\r\n");
        break;
      case ROTARY_RIGHT_CYCLE_SHORT_END:
        printf("Right cycle short end\r\n");
        break;
      case ROTARY_LEFT_CYCLE_ERROR: 
        printf("  ** Left cycle error **\r\n\r\n");
        break;    
      case ROTARY_RIGHT_CYCLE_ERROR:
        printf("  ** Right cycle error **\r\n\r\n");
        break;  
      default:
        printf("Action inconnue: %d\r\n", event);
    }
  }
}
#ifdef DOUBLE_TIME_ROTARY_ENCODER_DEBOUNCE
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  scanRotaryEncoder(GPIO_Pin);
}
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    scanRotaryEncoder(0);
  }
}

void scanRotaryEncoderSwitch()
{
  GPIO_PinState currentReading = HAL_GPIO_ReadPin(ROT_ENCOD_CLICK_GPIO_Port, ROT_ENCOD_CLICK_Pin);

  if (currentReading != lastButtonState)
  {
    // Le bouton a changé d'état, on reset le timer d'anti-rebond
    lastDebounceTime = HAL_GetTick();
  }

  if ((HAL_GetTick() - lastDebounceTime) > ROT_ENCOD_SW_DEBOUNCE_DELAY)
  {
    // Si le nouvel état est stable pendant DEBOUNCE_DELAY ms
    if (currentReading != buttonState)
    {
      buttonState = currentReading;

      if (buttonState == GPIO_PIN_RESET)
      {
        // Action à effectuer quand on appuie sur le bouton
        HAL_GPIO_TogglePin(GPIOE, LED_ON_BOARD_Pin);
      }
    }
  }

  lastButtonState = currentReading;
}

#ifdef __CLASSIC_DEBOUNCE
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint8_t etatA, etatB;
  if (GPIO_Pin == ROT_ENCOD_LEFT_Pin)
  {
    etatA = HAL_GPIO_ReadPin(ROT_ENCOD_LEFT_GPIO_Port, ROT_ENCOD_LEFT_Pin);
    etatB = HAL_GPIO_ReadPin(ROT_ENCOD_RIGHT_GPIO_Port, ROT_ENCOD_RIGHT_Pin);
    if (etatA == GPIO_PIN_SET)
      return; // ==>

    uint32_t now = HAL_GetTick();
    if (now - rot_encod_lastTick < ROT_ENCOD_DEBOUNCE_DELAY)
    {
      printf("Debounce !\r\n");
      rot_encod_lastTick = now;
      return; // Ignore l’interruption si trop rapide
    }
    rot_encod_lastTick = now;

    // Lire l’état de A et B ici, calculer le sens de rotation
    // et envoyer l’événement à une queue ou variable partagée

    // if (etatA != etatB) {
    // int8_t sens = (etatA && !etatB) ? 1 : -1;
    printf("Rotary gauche : %i \r\n", etatB);
    //} else {
    // printf("Rotary : NOT \r\n");
    // }
  }
  if (GPIO_Pin == ROT_ENCOD_RIGHT_Pin)
  {
    etatA = HAL_GPIO_ReadPin(ROT_ENCOD_LEFT_GPIO_Port, ROT_ENCOD_LEFT_Pin);
    etatB = HAL_GPIO_ReadPin(ROT_ENCOD_RIGHT_GPIO_Port, ROT_ENCOD_RIGHT_Pin);
    if (etatB == GPIO_PIN_SET)
      return; // ==>

    uint32_t now = HAL_GetTick();
    if (now - rot_encod_lastTick < ROT_ENCOD_DEBOUNCE_DELAY)
    {
      printf("Debounce !\r\n");
      rot_encod_lastTick = now;
      return; // Ignore l’interruption si trop rapide
    }
    rot_encod_lastTick = now;

    // if (etatA != etatB) {
    //  int8_t sens = (etatA && !etatB) ? 1 : -1;
    printf("Rotary Droite : %i \r\n", etatA);
    //} else {
    // printf("Rotary : NOT \r\n");
    // }
  }
}
#endif

#ifdef DOUBLE_TIME_ROTARY_ENCODER_DEBOUNCE
void scanRotaryEncoder(uint16_t GPIO_Pin)
{
  static uint8_t otherPinState;
  static uint8_t pinState;
  uint32_t now = HAL_GetTick();
  printf("Now: %lu, lastL: %lu, lastR: %lu \r\n", now, rot_encod_left_lastTick, rot_encod_right_lastTick);
  if (GPIO_Pin == ROT_ENCOD_LEFT_Pin)
  {
    pinState = HAL_GPIO_ReadPin(ROT_ENCOD_LEFT_GPIO_Port, ROT_ENCOD_LEFT_Pin);
    otherPinState = HAL_GPIO_ReadPin(ROT_ENCOD_RIGHT_GPIO_Port, ROT_ENCOD_RIGHT_Pin);

    if (now - rot_encod_left_lastTick < ROT_ENCOD_DEBOUNCE_DELAY)
    {
      rot_encod_left_lastTick = now;
      return; // Ignore interruption if too quick
    }
    rotary_log_event(ROTARY_L_LINE_FALL);
    rot_encod_left_lastTick = now;

    if (rotary_encoder_right_cycle_tstp > 0) {
      //if (now - rotary_encoder_right_cycle_tstp < ROT_ENCOD_CYCLE_MAX_DELAY) {
        if (otherPinState == GPIO_PIN_RESET) {
          rotary_log_event(ROTARY_R_END);
          // End of cycle
          rotary_encoder_right_cycle_tstp = 0;
        } else {
          rotary_log_event(ROTARY_BAD_R_END);
        }

      //} else {
      //  rotary_log_event(ROTARY_R_CYCLE_OUT_OF_TIME);
      //}
    } else {
      if (otherPinState == GPIO_PIN_SET) {
        rotary_log_event(ROTARY_L_BEGIN);
        rotary_encoder_left_cycle_tstp = now;
      } else {
        rotary_log_event(ROTARY_BAD_L_BEGIN);
      }
    }
  }

  if (GPIO_Pin == ROT_ENCOD_RIGHT_Pin)
  {
    pinState = HAL_GPIO_ReadPin(ROT_ENCOD_RIGHT_GPIO_Port, ROT_ENCOD_RIGHT_Pin);
    otherPinState = HAL_GPIO_ReadPin(ROT_ENCOD_LEFT_GPIO_Port, ROT_ENCOD_LEFT_Pin);

    if (now - rot_encod_right_lastTick < ROT_ENCOD_DEBOUNCE_DELAY)
    {
      // printf("  Debounce R %lu %lu %lu %i !\r\n", now, rot_encod_right_lastTick, now - rot_encod_right_lastTick, ROT_ENCOD_DEBOUNCE_DELAY);
      rot_encod_right_lastTick = now;
      return; // Ignore interruption if too quick
    }
    rot_encod_right_lastTick = now;
    rotary_log_event(ROTARY_R_LINE_FALL);

    if (rotary_encoder_left_cycle_tstp > 0) {
      //if (now - rotary_encoder_left_cycle_tstp < ROT_ENCOD_CYCLE_MAX_DELAY) {
        if (otherPinState == GPIO_PIN_RESET) {
          rotary_log_event(ROTARY_L_END);
          // End of cycle
          rotary_encoder_left_cycle_tstp = 0;
        } else {
          rotary_log_event(ROTARY_BAD_L_END);
        }

      //} else {
      //  rotary_log_event(ROTARY_L_CYCLE_OUT_OF_TIME);
      //}
    } else {
      if (otherPinState == GPIO_PIN_SET) {
        rotary_log_event(ROTARY_R_BEGIN);
        rotary_encoder_right_cycle_tstp = now;
      } else {
        rotary_log_event(ROTARY_BAD_R_BEGIN);
      }
    }
    //printf("</droit>\r\n");
  }
}
#endif

#ifdef DOUBLE_TIME_ROTARY_ENCODER_TIMER
uint8_t leftCycleState = 0;
uint32_t leftCycleTstp = 0;
uint8_t rightCycleState = 0;
uint32_t rightCycleTstp = 0;

uint8_t leftCycleNb = 0;
uint8_t rightCycleNb = 0;

void scanRotaryEncoder(uint16_t GPIO_Pin_dummy)
{
  uint8_t leftPinState;
  uint8_t rightPinState;
  uint32_t now = HAL_GetTick();


  leftPinState = HAL_GPIO_ReadPin(ROT_ENCOD_LEFT_GPIO_Port, ROT_ENCOD_LEFT_Pin);
  rightPinState = HAL_GPIO_ReadPin(ROT_ENCOD_RIGHT_GPIO_Port, ROT_ENCOD_RIGHT_Pin);
  uint8_t leftAndRightPinState = (leftPinState << 1) | rightPinState;
  //printf("Now: %lu %i %i %i \r\n", now, leftAndRightPinState, leftCycleNb, rightCycleNb);
  if (leftCycleState == 0 && rightCycleState == 0 && leftAndRightPinState == 0b01) {
    rotary_log_event(ROTARY_LEFT_CYCLE_BEGIN);
    leftCycleTstp = now;
    leftCycleState = 1; // Cycle began
  }
  if (leftCycleState == 1) {
    if (leftAndRightPinState == 0b00) {
      rotary_log_event(ROTARY_LEFT_CYCLE_1);
      leftCycleState = 2; // Second step of left cycle
    }
    if (leftAndRightPinState == 0b11) { // Error state
      rotary_log_event(ROTARY_LEFT_CYCLE_ERROR);
      leftCycleState = 0; // Reset state
      leftCycleTstp = 0; // Reset timestamp
    }
  }

  if (leftCycleState == 2) {
    if (leftAndRightPinState == 0b10) {
      rotary_log_event(ROTARY_LEFT_CYCLE_2);
      leftCycleState = 3; // Third step of left cycle
    }
    if (leftAndRightPinState == 0b11) {
      rotary_log_event(ROTARY_LEFT_CYCLE_SHORT_END);
      leftCycleState = 0; // Cycle ended
      leftCycleTstp = 0; // Reset timestamp
    }
  }
  if (leftCycleState == 3 && leftAndRightPinState == 0b11) {
    rotary_log_event(ROTARY_LEFT_CYCLE_END);
    leftCycleState = 0; // Cycle ended
    leftCycleTstp = 0; // Reset timestamp
  }

  if (leftCycleState == 0 && rightCycleState == 0) {
    if (leftAndRightPinState == 0b10) {
      rotary_log_event(ROTARY_RIGHT_CYCLE_BEGIN);
      rightCycleTstp = now;
      rightCycleState = 1; // Cycle began
    }
  }
  if (rightCycleState == 1) {
    if (leftAndRightPinState == 0b00) {
      rotary_log_event(ROTARY_RIGHT_CYCLE_1);
      rightCycleState = 2; // Second step of right cycle  
    }
    
    if (leftAndRightPinState == 0b11) { // Error state
      rotary_log_event(ROTARY_RIGHT_CYCLE_ERROR);
      rightCycleState = 0; // Reset state
      rightCycleTstp = 0; // Reset timestamp
    }
  } 
  if (rightCycleState == 2) {
    if (leftAndRightPinState == 0b01) {
      rotary_log_event(ROTARY_RIGHT_CYCLE_2);
      rightCycleState = 3; // Third step of right cycle
    }
    if (leftAndRightPinState == 0b11) { // Kind of Error state..but we accept it
      rotary_log_event(ROTARY_RIGHT_CYCLE_SHORT_END);
      rightCycleState = 0; // Cycle ended
      rightCycleTstp = 0; // Reset timestamp
    }
  }
  if (rightCycleState == 3 && leftAndRightPinState == 0b11) {
    rotary_log_event(ROTARY_RIGHT_CYCLE_END);
    rightCycleState = 0; // Cycle ended
    rightCycleTstp = 0; // Reset timestamp
  }
}
#endif
