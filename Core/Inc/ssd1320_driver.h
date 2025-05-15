#ifndef INC_SSD1320_DRIVER_H_
#define INC_SSD1320_DRIVER_H_


#include <stdint.h>
#include <stddef.h>

#define SSD1320_CS1_LOW()     HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define SSD1320_CS1_HIGH()    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
#define SSD1320_CS2_LOW()     HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET)
#define SSD1320_CS2_HIGH()    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET)

#define SSD1320_DC_CMD()     HAL_GPIO_WritePin(SPI12_DC_GPIO_Port, SPI12_DC_Pin, GPIO_PIN_RESET)
#define SSD1320_DC_DATA()    HAL_GPIO_WritePin(SPI12_DC_GPIO_Port, SPI12_DC_Pin, GPIO_PIN_SET)
#define SSD1320_RST_LOW()    HAL_GPIO_WritePin(SPI12_RESET_GPIO_Port, SPI12_RESET_Pin, GPIO_PIN_RESET)
#define SSD1320_RST_HIGH()   HAL_GPIO_WritePin(SPI12_RESET_GPIO_Port, SPI12_RESET_Pin, GPIO_PIN_SET)

#define OLED_CMD  0 //COMMAND
#define OLED_DATA 1 //DATA

void SSD1320_Init(void);
void SSD1320_SendCommand(uint8_t cmd);
void SSD1320_SendData(uint8_t* data, size_t len);
void SSD1320_SendBuffer(uint8_t* buffer, size_t len);
void OLED_Clear(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint8_t color);
void SSD1320_SetAddress(uint16_t x1,uint16_t x2,uint16_t y1,uint16_t y2);


#endif /* INC_SSD1320_DRIVER_H_ */
