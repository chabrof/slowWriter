#include "rotary_encoder.h"
#include <stdint.h>
#include <string.h>
#include <stm32h723xx.h>
#include <stdio.h>

uint32_t rot_encod_lastTick = 0;

// Switch scan
uint32_t lastDebounceTime = 0;
GPIO_PinState lastButtonState = GPIO_PIN_SET;
GPIO_PinState buttonState = GPIO_PIN_SET;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  scanRotaryEncoder(GPIO_Pin);
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

#ifdef __FULL_CYCLE_DEBOUNCE
// Variables
int etatPrecedentLigneCLK; // Cette variable nous permettra de stocker le dernier état de la ligne CLK, afin de le comparer à l'actuel
int etatPrecedentLigneDT;  // Cette variable nous permettra de stocker le dernier état de la ligne DT, afin de le comparer à l'actuel

int compteur = 0; // Cette variable nous permettra de compter combien de crans ont été parcourus, sur l'encodeur
                  // (sachant que nous compterons dans le sens horaire, et décompterons dans le sens antihoraire)
void init()
{
  // Initialisation de la liaison série (arduino nano <-> PC)

  // Petite pause pour laisser le temps aux signaux de se stabiliser
  delay(200);

  // Mémorisation des valeurs initiales des lignes SW/CLK/DT, au démarrage du programme
  // etatPrecedentLigneSW  = digitalRead(pinArduinoRaccordementSignalSW);
  etatPrecedentLigneCLK = digitalRead(pinArduinoRaccordementSignalCLK);
  etatPrecedentLigneDT = digitalRead(pinArduinoRaccordementSignalDT);

  // Affichage de la valeur initiale du compteur, sur le moniteur série
  printF("Valeur initiale du compteur = %i", compteur);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  // Lecture des lignes CLK et DT, issue du KY-040, arrivant sur l'arduino
  int etatActuelDeLaLigneCLK = digitalRead(pinArduinoRaccordementSignalCLK);
  int etatActuelDeLaLigneDT = digitalRead(pinArduinoRaccordementSignalDT);

  // ************************
  // * CAS : Incrémentation *
  // ************************
  // Si CLK = 1 et DT = 0, et que l'ancienCLK = 0 et ancienDT = 1, alors le bouton a été tourné d'un cran vers la droite (sens horaire, donc incrémentation)
  if ((etatActuelDeLaLigneCLK == HIGH) && (etatActuelDeLaLigneDT == LOW) && (etatPrecedentLigneCLK == LOW) && (etatPrecedentLigneDT == HIGH))
  {

    // Alors on incrémente le compteur
    compteur++;

    // Et on affiche ces infos sur le moniteur série
    printF("Sens = horaire | Valeur du compteur = %i ", compteur);
  }

  // ************************
  // * CAS : Décrémentation *
  // ************************
  // Si CLK = 1 et DT = 1, et que l'ancienCLK = 0 et ancienDT = 0, alors le bouton a été tourné d'un cran vers la gauche (sens antihoraire, donc décrémentation)
  if ((etatActuelDeLaLigneCLK == HIGH) && (etatActuelDeLaLigneDT == HIGH) && (etatPrecedentLigneCLK == LOW) && (etatPrecedentLigneDT == LOW))
  {

    // Alors on décrémente le compteur
    compteur--;

    // Et on affiche ces infos sur le moniteur série
    printf("Sens = antihoraire | Valeur du compteur = %i", compteur);
  }

  // Et on mémorise ces états actuels comme étant "les nouveaux anciens", pour le "tour suivant" !
  etatPrecedentLigneCLK = etatActuelDeLaLigneCLK;
  etatPrecedentLigneDT = etatActuelDeLaLigneDT;
}
#endif

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
  if (GPIO_Pin == ROT_ENCOD_LEFT_Pin)
  {
    uint32_t now = HAL_GetTick();
    if (now - rot_encod_lastTick < ROT_ENCOD_DEBOUNCE_DELAY)
    {
      printf("Debounce !\r\n");
      rot_encod_lastTick = now;
      return; // Ignore interruption if too quick
    }
    rot_encod_lastTick = now;
    otherPinState = HAL_GPIO_ReadPin(ROT_ENCOD_RIGHT_GPIO_Port, ROT_ENCOD_RIGHT_Pin);
  }


}
#endif
