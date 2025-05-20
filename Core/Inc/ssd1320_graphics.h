#ifndef INC_SSD1320_GRAPHICS_H_
#define INC_SSD1320_GRAPHICS_H_

#include <stdint.h>
#include <stddef.h>

#define SSD1320_WIDTH  320
#define SSD1320_HEIGHT 132

void SetPixel4BPP(uint16_t x, uint16_t y, uint8_t gray);
void DrawChar4BPP(char c, uint16_t x, uint16_t y, uint8_t color);
void DrawText4BPP(const char* text, uint16_t x, uint16_t y, uint8_t color);
void DrawRect4BPP(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color, uint8_t filled);
void DrawLine4BPP(int x0, int y0, int x1, int y1, uint8_t color);
void ClearBuffers();


#endif /* INC_SSD1320_GRAPHICS_H_ */
