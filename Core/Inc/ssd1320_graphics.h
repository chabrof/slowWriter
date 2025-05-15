#ifndef INC_SSD1320_GRAPHICS_H_
#define INC_SSD1320_GRAPHICS_H_

#include <stdint.h>
#include <stddef.h>

#define SSD1320_WIDTH    160
#define SSD1320_HEIGHT      132
#define SSD1320_BUF_SIZE  (SSD1320_WIDTH * SSD1320_HEIGHT / 2) // 4bpp

void SetPixel4BPP(uint8_t* buffer, uint16_t x, uint16_t y, uint8_t gray);
void DrawChar4BPP(uint8_t* buffer, char c, uint16_t x, uint16_t y, uint8_t color);
void DrawText4BPP(uint8_t* buffer, const char* text, uint16_t x, uint16_t y, uint8_t color);
void DrawRect4BPP(uint8_t* buffer, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color, uint8_t filled);
void DrawLine4BPP(uint8_t* buffer, int x0, int y0, int x1, int y1, uint8_t color);
void ClearBuffer(uint8_t* buffer);


#endif /* INC_SSD1320_GRAPHICS_H_ */
