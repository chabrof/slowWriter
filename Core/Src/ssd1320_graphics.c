#include "ssd1320_graphics.h"
#include <string.h>
#include <stdlib.h>

// ===== POLICE BASIQUE 6x8 (ASCII 32–127) =====
const uint8_t font6x8[][6] = {
    [32] = {0x00,0x00,0x00,0x00,0x00,0x00}, // Espace
    [65] = {0x7C,0x12,0x11,0x12,0x7C,0x00}, // A
    [66] = {0x7F,0x49,0x49,0x49,0x36,0x00}, // B
    [67] = {0x3E,0x41,0x41,0x41,0x22,0x00}, // C
    // Ajoute les caractères utiles
};

// ===== GRAPHISME DE BASE =====
void SetPixel4BPP(uint8_t* buffer, uint16_t x, uint16_t y, uint8_t gray)
{
    if (x >= SSD1320_WIDTH || y >= SSD1320_HEIGHT)
        return;

    uint32_t index = (y * SSD1320_WIDTH + x) / 2;

    if (x % 2 == 0)
        buffer[index] = (buffer[index] & 0x0F) | ((gray & 0x0F) << 4);
    else
        buffer[index] = (buffer[index] & 0xF0) | (gray & 0x0F);
}

void DrawChar4BPP(uint8_t* buffer, char c, uint16_t x, uint16_t y, uint8_t color)
{
    if (c < 32 || c > 127) return;
    const uint8_t* chr = font6x8[(uint8_t)c];

    for (uint8_t col = 0; col < 6; col++) {
        uint8_t line = chr[col];
        for (uint8_t row = 0; row < 8; row++) {
            if (line & (1 << row)) {
                SetPixel4BPP(buffer, x + col, y + row, color);
            }
        }
    }
}

void DrawText4BPP(uint8_t* buffer, const char* text, uint16_t x, uint16_t y, uint8_t color)
{
    while (*text) {
        DrawChar4BPP(buffer, *text, x, y, color);
        x += 6;
        text++;
    }
}

void DrawRect4BPP(uint8_t* buffer, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color, uint8_t filled)
{
    for (uint16_t i = 0; i < h; i++) {
        for (uint16_t j = 0; j < w; j++) {
            if (filled || i == 0 || i == h-1 || j == 0 || j == w-1) {
                SetPixel4BPP(buffer, x + j, y + i, color);
            }
        }
    }
}

void DrawLine4BPP(uint8_t* buffer, int x0, int y0, int x1, int y1, uint8_t color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        SetPixel4BPP(buffer, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void ClearBuffer(uint8_t* buffer)
{
    memset(buffer, 0x00, SSD1320_BUF_SIZE);
}
