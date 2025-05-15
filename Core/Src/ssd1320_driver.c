#include "ssd1320_driver.h"
#include "main.h" // pour hspi1

extern SPI_HandleTypeDef hspi1;

void SSD1320_Reset(void)
{
    SSD1320_RST_LOW();
    HAL_Delay(200);
    SSD1320_RST_HIGH();
    HAL_Delay(200);
}

void SSD1320_SendCommandLeft(uint8_t cmd)
{
    SSD1320_DC_CMD();
    SSD1320_CS1_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    SSD1320_CS1_HIGH();
}

void SSD1320_SendDataLeft(uint8_t* data, size_t len)
{
    SSD1320_DC_DATA();
    SSD1320_CS1_LOW();
    HAL_SPI_Transmit_DMA(&hspi1, data, len);
    // HAL_SPI_Transmit(&hspi1, data, len, HAL_MAX_DELAY);
    //SSD1320_CS1_HIGH();
}

void SSD1320_SendCommandRight(uint16_t cmd)
{
    SSD1320_DC_CMD();
    SSD1320_CS2_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    SSD1320_CS2_HIGH();
}

void SSD1320_SendDataRight(uint8_t* data, size_t len)
{
    SSD1320_DC_DATA();
    SSD1320_CS2_LOW();
    HAL_SPI_Transmit_DMA(&hspi1, data, len);
    //HAL_SPI_Transmit(&hspi1, data, len, HAL_MAX_DELAY);
    //SSD1320_CS2_HIGH();
}

void SSD1320_SendCommandBoth(uint8_t cmd) {
  SSD1320_DC_CMD();
  SSD1320_CS1_LOW();
  SSD1320_CS2_LOW();
  HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
  SSD1320_CS1_HIGH();
  SSD1320_CS2_HIGH();
}

void SSD1320_SendDataBoth(uint8_t* data, size_t len)
{
    SSD1320_DC_DATA();
    SSD1320_CS1_LOW();
    SSD1320_CS2_LOW();
    HAL_SPI_Transmit_DMA(&hspi1, data, len);
    //HAL_SPI_Transmit(&hspi1, data, len, HAL_MAX_DELAY);
    //SSD1320_CS1_HIGH();
    //SSD1320_CS2_HIGH();
}


// End of DMA transfert cbk :
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI1) {
    SSD1320_CS1_HIGH();
    SSD1320_CS2_HIGH();
  }
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

void SSD1320_SendBuffer(uint8_t* buffer, size_t len)
{
  // Positionne la fenêtre d’écriture sur toute la surface
  //SSD1320_SetColumnAddressLeft(0x00, 0x50); // 160 px = 80 octets (en 4 bpp)
  //SSD1320_SetRowAddressLeft(0x00, 0x83);    // 132 lignes
  //SSD1320_SetAddress(0,319,0,131);
  SSD1320_SendDataBoth(buffer, len);

  // Positionne la fenêtre d’écriture sur toute la surface
  //SSD1320_SetColumnAddressRight(0x00, 0x50); // 160 px = 80 octets (en 4 bpp)
  //SSD1320_SetRowAddressRight(0x00, 0x83);    // 132 lignes
  //SSD1320_SetAddress(160,319,0,131);
}



/*
void SSD1320_Init(void)
{
    SSD1320_Reset();

    SSD1320_SendCommand(0xAE); // Display OFF
    SSD1320_SendCommand(0x15); SSD1320_SendCommand(0x00); SSD1320_SendCommand(0x3F); // Columns
    SSD1320_SendCommand(0x75); SSD1320_SendCommand(0x00); SSD1320_SendCommand(0x3F); // Rows
    SSD1320_SendCommand(0xA0); SSD1320_SendCommand(0x14); // Remap
    SSD1320_SendCommand(0xA1); SSD1320_SendCommand(0x00); // Start Line
    SSD1320_SendCommand(0xA2); SSD1320_SendCommand(0x00); // Offset
    SSD1320_SendCommand(0xA4); // Display mode
    SSD1320_SendCommand(0xAB); SSD1320_SendCommand(0x01); // VDD internal
    SSD1320_SendCommand(0x81); SSD1320_SendCommand(0x53); // Contrast
    SSD1320_SendCommand(0xB1); SSD1320_SendCommand(0x51); // Phase length
    SSD1320_SendCommand(0xB3); SSD1320_SendCommand(0x01); // Clock
    SSD1320_SendCommand(0xB9); // Default grayscale
    SSD1320_SendCommand(0xBC); SSD1320_SendCommand(0x08); // Precharge voltage
    SSD1320_SendCommand(0xBE); SSD1320_SendCommand(0x07); // VCOMH
    SSD1320_SendCommand(0xAF); // Display ON
}*/

void SSD1320_Init(void)
{
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

/*
void SSD1320_Init(void)
{
  // RESET matériel
  SSD1320_Reset();

  SSD1320_SendCommandLeft(0xAE); // Display OFF

  SSD1320_SendCommandLeft(0xFD); // Set Command Lock
  SSD1320_SendCommandLeft(0x12);

  SSD1320_SendCommandLeft(0x20); // Set Memory Addressing Mode
  SSD1320_SendCommandLeft(0x00); // Horizontal Addressing Mode

  SSD1320_SendCommandLeft(0x25); // Portrait Addressing Mode
  SSD1320_SendCommandLeft(0x00); // Normal

  SSD1320_SendCommandLeft(0x81); // Contrast Control
  SSD1320_SendCommandLeft(0x90);

  SSD1320_SendCommandLeft(0xA0); // SEG Remap
  SSD1320_SendCommandLeft(0x00); // false

  SSD1320_SendCommandLeft(0xA1);
  SSD1320_SendCommandLeft(0x02);

  SSD1320_SendCommandLeft(0xA2); // Display Start Line
  SSD1320_SendCommandLeft(0x00);

  SSD1320_SendCommandLeft(0xA4); // Resume to RAM content
  SSD1320_SendCommandLeft(0xA6); // Normal Display (not inverted)

  SSD1320_SendCommandLeft(0xA8); // MUX Ratio
  SSD1320_SendCommandLeft(0x83); // 1/132 duty

  SSD1320_SendCommandLeft(0xAD); // Internal/External IREF
  SSD1320_SendCommandLeft(0x10); // External

  SSD1320_SendCommandLeft(0xBC); // Pre-charge voltage
  SSD1320_SendCommandLeft(0x1E);

  SSD1320_SendCommandLeft(0xBF); // Linear LUT

  SSD1320_SendCommandLeft(0xC8); // COM Output Scan
  //SSD1320_SendCommandLeft(0x01);

  //SSD1320_SendCommandLeft(0xC0);
  //SSD1320_SendCommandLeft(0x02);

  SSD1320_SendCommandLeft(0xD3); // Display Offset
  SSD1320_SendCommandLeft(0x0E);

  SSD1320_SendCommandLeft(0x92);
  SSD1320_SendCommandLeft(0x02);

  SSD1320_SendCommandLeft(0xD5); // Clock Divide / Oscillator Freq
  SSD1320_SendCommandLeft(0xC2); // Hz

  SSD1320_SendCommandLeft(0xD9); // Pre-charge Period
  SSD1320_SendCommandLeft(0x72);

  SSD1320_SendCommandLeft(0xDA); // SEG Pins Hardware Config
  SSD1320_SendCommandLeft(0x32);

  SSD1320_SendCommandLeft(0xBD); // VP (Voltage Level)
  SSD1320_SendCommandLeft(0x03);

  SSD1320_SendCommandLeft(0xDB); // VCOMH
  SSD1320_SendCommandLeft(0x35);

  // Effacer tout l'écran avant l’allumage
  //OLED_Clear(0, 0, 320, 132, 0x00); // à adapter selon ta lib graphique

  SSD1320_SendCommandLeft(0xAF); // Display ON


  SSD1320_SendCommandRight(0xAE); // Display OFF

  SSD1320_SendCommandRight(0xFD); // Set Command Lock
  SSD1320_SendCommandRight(0x12);

  SSD1320_SendCommandRight(0x20); // Set Memory Addressing Mode
  SSD1320_SendCommandRight(0x00); // Horizontal Addressing Mode

  SSD1320_SendCommandRight(0x25); // Portrait Addressing Mode
  SSD1320_SendCommandRight(0x00); // Normal

  SSD1320_SendCommandRight(0x81); // Contrast Control
  SSD1320_SendCommandRight(0x90);

  SSD1320_SendCommandRight(0xA0); // SEG Remap
  SSD1320_SendCommandRight(0x01); // true

  SSD1320_SendCommandRight(0xA1);
  SSD1320_SendCommandRight(0x02);

  SSD1320_SendCommandRight(0xA2); // Display Start Line
  SSD1320_SendCommandRight(0x00);

  SSD1320_SendCommandRight(0xA4); // Resume to RAM content
  SSD1320_SendCommandRight(0xA6); // Normal Display (not inverted)

  SSD1320_SendCommandRight(0xA8); // MUX Ratio
  SSD1320_SendCommandRight(0x83); // 1/132 duty

  SSD1320_SendCommandRight(0xAD); // Internal/External IREF
  SSD1320_SendCommandRight(0x10); // External

  SSD1320_SendCommandRight(0xBC); // Pre-charge voltage
  SSD1320_SendCommandRight(0x1E);

  SSD1320_SendCommandRight(0xBF); // Linear LUT

  SSD1320_SendCommandRight(0xC8); // COM Output Scan
  //SSD1320_SendCommandRight(0x01);

  SSD1320_SendCommandRight(0xC0);
  //SSD1320_SendCommandRight(0x02);

  SSD1320_SendCommandRight(0xD3); // Display Offset
  //SSD1320_SendCommandRight(0x92);
  SSD1320_SendCommandRight(0x92);


  SSD1320_SendCommandRight(0x75); // Row Address
  SSD1320_SendCommandRight(0x00);
  SSD1320_SendCommandRight(0x83); // Jusqu’à ligne 131 (132 lignes)


  SSD1320_SendCommandRight(0x92);
  SSD1320_SendCommandRight(0x02);

  SSD1320_SendCommandRight(0xD5); // Clock Divide / Oscillator Freq
  SSD1320_SendCommandRight(0xA2); // Hz

  SSD1320_SendCommandRight(0xD9); // Pre-charge Period
  SSD1320_SendCommandRight(0x72);

  SSD1320_SendCommandRight(0xDA); // SEG Pins Hardware Config
  SSD1320_SendCommandRight(0x32);

  SSD1320_SendCommandRight(0xBD); // VP (Voltage Level)
  SSD1320_SendCommandRight(0x03);

  SSD1320_SendCommandRight(0xDB); // VCOMH
  SSD1320_SendCommandRight(0x30);

  // Effacer tout l'écran avant l’allumage
  //OLED_Clear(0, 0, 320, 132, 0x00); // à adapter selon ta lib graphique

  SSD1320_SendCommandRight(0xAF); // Display ON
}*/
