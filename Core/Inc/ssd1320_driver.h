#ifndef INC_SSD1320_DRIVER_H_
#define INC_SSD1320_DRIVER_H_

#include <stdint.h>
#include <stddef.h>


#define SSD1320_HALF_WIDTH  160 // for left and right screens
#define SSD1320_HALF_HEIGHT 132
#define SSD1320_BUF_SIZE  (SSD1320_HALF_WIDTH * SSD1320_HALF_HEIGHT / 2) // 4bpp (1 octet = 2 dots)


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

#define DMA_TRANSFER_NONE 0
#define DMA_TRANSFER_LEFT 1
#define DMA_TRANSFER_RIGHT 2

#define BUFFER_SENT 2
#define BUFFER_SENDING 1
#define BUFFER_NOT_SENT 0

typedef struct {
    uint8_t left_screen_status;
    uint8_t* left_screen_data;
    uint8_t right_screen_status;
    uint8_t* right_screen_data;
} T_Frame_Buffer;

// Double Frame buffer
extern T_Frame_Buffer* buffers;

extern uint8_t cur_buffer_idx_to_send;
extern uint8_t cur_buffer_idx_to_paint;

void SSD1320_Init(void);
void SSD1320_SendCommandLeft(uint8_t cmd);
void SSD1320_SendCommandRight(uint8_t cmd);
void SSD1320_SendCommandBoth(uint8_t cmd);
void SSD1320_SendBuffer_Left();
void SSD1320_SendBuffer_Right();
void OLED_Clear(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint8_t color);
void SSD1320_SetAddress(uint16_t x1,uint16_t x2,uint16_t y1,uint16_t y2);


//extern uint8_t* ssd1320_left_buffer;
//extern uint8_t* ssd1320_right_buffer;

#endif /* INC_SSD1320_DRIVER_H_ */
