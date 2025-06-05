#include "ssd1320_driver.h"
#include "main.h" // pour hspi1
#include "stdio.h"

extern SPI_HandleTypeDef hspi1;
volatile uint8_t dma_transfer_state = 0;

uint8_t frame_buffer_a[SSD1320_BUF_SIZE];
uint8_t frame_buffer_b[SSD1320_BUF_SIZE];
uint8_t *ssd1320_left_buffer = NULL;
uint8_t left_buffer_sending_status = BUFFER_NOT_SENT;
uint8_t *ssd1320_right_buffer = NULL;
uint8_t right_buffer_sending_status = BUFFER_NOT_SENT;

void SSD1320_Reset(void)
{
    SSD1320_RST_LOW();
    HAL_Delay(100);
    SSD1320_RST_HIGH();
    HAL_Delay(100);
}

void SSD1320_SendCommandLeft(uint8_t cmd)
{
    SSD1320_DC_CMD();
    SSD1320_CS1_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    SSD1320_CS1_HIGH();
}

void SSD1320_SendCommandRight(uint8_t cmd)
{
    SSD1320_DC_CMD();
    SSD1320_CS2_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    SSD1320_CS2_HIGH();
}

void SSD1320_SendCommandBoth(uint8_t cmd) {
  SSD1320_DC_CMD();
  SSD1320_CS1_LOW();
  SSD1320_CS2_LOW();
  HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
  SSD1320_CS1_HIGH();
  SSD1320_CS2_HIGH();
}


void SSD1320_SetColumnAddressLeft(uint8_t start, uint8_t end)
{
    SSD1320_SendCommandLeft(0x21);
    SSD1320_SendCommandLeft(start);
    SSD1320_SendCommandLeft(end);
}

void SSD1320_SetRowAddressLeft(uint8_t start, uint8_t end)
{
    SSD1320_SendCommandLeft(0x22);
    SSD1320_SendCommandLeft(start);
    SSD1320_SendCommandLeft(end);
}

void SSD1320_SetColumnAddressRight(uint8_t start, uint8_t end)
{
    SSD1320_SendCommandRight(0x21);
    SSD1320_SendCommandRight(start);
    SSD1320_SendCommandRight(end);
}

void SSD1320_SetRowAddressRight(uint8_t start, uint8_t end)
{
    SSD1320_SendCommandRight(0x22);
    SSD1320_SendCommandRight(start);
    SSD1320_SendCommandRight(end);
}

void OLED_Clear(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint8_t color)
{
  /*uint8_t t=0,sizex;
  uint8_t xt=0;
  x1/=2;
  x2/=2;
  sizex=x2-x1;
  */if(x1<80){}
  else
  {
    x1-=80;
  }
  SSD1320_SetAddress(x1,x2-1,y1,y2-1);
}

void SSD1320_SetAddress(uint16_t x1,uint16_t x2,uint16_t y1,uint16_t y2)
{
  //if(0 && x2<80)
  //{
    SSD1320_SendCommandLeft(0x21);
    SSD1320_SendCommandLeft(x1);
    SSD1320_SendCommandLeft(x2);
    SSD1320_SendCommandLeft(0x22);
    SSD1320_SendCommandLeft(y1);
    SSD1320_SendCommandLeft(y2);

    SSD1320_SendCommandRight(0x21);
    SSD1320_SendCommandRight(x1);
    SSD1320_SendCommandRight(x2);
    SSD1320_SendCommandRight(0x22);
    SSD1320_SendCommandRight(y1);
    SSD1320_SendCommandRight(y2);
  /*}
  else
  {
    SSD1320_SendCommandLeft(0x21);
    SSD1320_SendCommandLeft(x1);
    SSD1320_SendCommandLeft(79);
    SSD1320_SendCommandLeft(0x22);
    SSD1320_SendCommandLeft(y1);
    SSD1320_SendCommandLeft(y2);

    SSD1320_SendCommandRight(0x21);
    SSD1320_SendCommandRight(0);
    SSD1320_SendCommandRight(x2-80);
    //SSD1320_SendCommandRight(160);
    //SSD1320_SendCommandRight(x2);

    SSD1320_SendCommandRight(0x22);
    SSD1320_SendCommandRight(y1);
    SSD1320_SendCommandRight(y2);
  }*/
}


// End of DMA transfert cbk :
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  printf("DMA cbk. (%i)\r\n", dma_transfer_state);
  if (hspi->Instance == SPI1) {
    if (dma_transfer_state == DMA_TRANSFER_NONE) {\
      // Not very useful, but just in case :
      SSD1320_CS1_HIGH();
      SSD1320_CS2_HIGH();
      return;
    }
    if (dma_transfer_state == DMA_TRANSFER_LEFT) {
      dma_transfer_state = DMA_TRANSFER_NONE;
      SSD1320_CS1_HIGH();
      left_buffer_sending_status = BUFFER_SENT;
      printf("Transmission gauche terminee %i \r\n", SSD1320_BUF_SIZE);
      return;
    }
    if (dma_transfer_state == DMA_TRANSFER_RIGHT) {
      dma_transfer_state = DMA_TRANSFER_NONE;
      SSD1320_CS2_HIGH();
      right_buffer_sending_status = BUFFER_SENT;
      printf("Transmission droite terminee %i \r\n", SSD1320_BUF_SIZE);
      return;
    }
  }
}

void SSD1320_SendBuffer_Left()
{
  if (left_buffer_sending_status == BUFFER_SENT) {
    return;
  } 
  while (dma_transfer_state != DMA_TRANSFER_NONE); // Attendre que le DMA soit inactif
  dma_transfer_state = DMA_TRANSFER_LEFT;
  printf("Transmission gauche %i \r\n", SSD1320_BUF_SIZE);
  SSD1320_DC_DATA();
  SSD1320_CS1_LOW();
  HAL_SPI_Transmit_DMA(&hspi1, ssd1320_left_buffer, SSD1320_BUF_SIZE);
}

void SSD1320_SendBuffer_Right()
{
  if (right_buffer_sending_status == BUFFER_SENT) {
    return;
  } 
  while (dma_transfer_state != DMA_TRANSFER_NONE); // Attendre que le DMA soit inactif
  dma_transfer_state = DMA_TRANSFER_RIGHT;
  printf("Transmission droite %i \r\n", SSD1320_BUF_SIZE);
  SSD1320_DC_DATA();
  SSD1320_CS2_LOW();
  HAL_SPI_Transmit_DMA(&hspi1, ssd1320_right_buffer, SSD1320_BUF_SIZE);
}


void SSD1320_Init(void)
{
  // Buffers creation
  ssd1320_left_buffer = frame_buffer_a;
  ssd1320_right_buffer = frame_buffer_b;

  SSD1320_Reset();

  SSD1320_SendCommandBoth(0xae);//Display OFF
  SSD1320_SendCommandBoth(0xfd);//Set Command Lock
  SSD1320_SendCommandBoth(0x12);
  SSD1320_SendCommandBoth(0x20);//Set Memory Addressing Mode
  SSD1320_SendCommandBoth(0x00);
  SSD1320_SendCommandBoth(0x25);//Set Portrait Addressing Mode
  SSD1320_SendCommandBoth(0x00);//Normal Addressing Mode
  SSD1320_SendCommandBoth(0x81);//Set Contrast Control
  SSD1320_SendCommandBoth(0x85);
  SSD1320_SendCommandLeft(0xa0);//Set Seg Remap
  SSD1320_SendCommandRight(0xa1);
  SSD1320_SendCommandBoth(0xa2);//Set Display Start Line
  SSD1320_SendCommandBoth(0x00);
  SSD1320_SendCommandBoth(0xa4);//Resume to RAM content display
  SSD1320_SendCommandBoth(0xa6);//Set Normal Display
  SSD1320_SendCommandBoth(0xa8);//Set MUX Ratio
  SSD1320_SendCommandBoth(0x83);//1/132 duty
  SSD1320_SendCommandBoth(0xad);//Select external or internal IREF
  SSD1320_SendCommandBoth(0x10);
  SSD1320_SendCommandBoth(0xbc);//Set Pre-charge voltage
  SSD1320_SendCommandBoth(0xA6);//
  SSD1320_SendCommandBoth(0xbf);//Linear LUT
  SSD1320_SendCommandLeft(0xc8);//Set COM Output Scan Direction
  SSD1320_SendCommandRight(0xc0);
  SSD1320_SendCommandBoth(0xd3);//Set Display Offset
  SSD1320_SendCommandLeft(0x0e);
  //SSD1320_SendCommandRight(0xD2);
  SSD1320_SendCommandRight(0x92);
  SSD1320_SendCommandBoth(0xd5);//Set Display Clock Divide Ratio/Oscillator Frequency
  SSD1320_SendCommandBoth(0x52);//85Hz
  SSD1320_SendCommandBoth(0xd9);//Set Pre-charge Period
  SSD1320_SendCommandBoth(0x52);//
  SSD1320_SendCommandBoth(0xda);//Set SEG Pins Hardware Configuration
  SSD1320_SendCommandBoth(0x32);
  SSD1320_SendCommandBoth(0xbd);//Set VP
  SSD1320_SendCommandBoth(0x03);
  SSD1320_SendCommandBoth(0xdb);//Set VCOMH
  SSD1320_SendCommandBoth(0x80);


  SSD1320_SendCommandBoth(0xaf);//Display on
}

