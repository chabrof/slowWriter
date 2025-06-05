#include "ssd1320_driver.h"
#include "ssd1320_graphics.h"
#include <string.h>
#include <stdlib.h>
#include "font8x8_basic.h"

void SetPixel4BPP(uint16_t x, uint16_t y, uint8_t gray)
{
  if (x >= SSD1320_WIDTH || y >= SSD1320_HEIGHT)
    return;

  uint8_t *buf;
  uint16_t local_x;

  // Choisir le buffer selon la moitié
  if (x < SSD1320_HALF_WIDTH) {
    buf = ssd1320_left_buffer;
    local_x = x;
  } else {
    buf = ssd1320_right_buffer;
    local_x = x - 160;
  }

  // Calcul de l'adresse dans le buffer (2 pixels par octet, 4bpp)
  uint32_t byte_index = y * 80 + (local_x / 2);
  uint8_t pixel = buf[byte_index];

  if (local_x % 2 == 0) {
    // Pixel pair : bits hauts
    pixel = (pixel & 0x0F) | (gray << 4);
  } else {
    // Pixel impair : bits bas
    pixel = (pixel & 0xF0) | (gray & 0x0F);
  }

  buf[byte_index] = pixel;
}

void DrawChar4BPP(char c, uint16_t x, uint16_t y, uint8_t color)
{
  if (c < 0x20 || c > 0x7F) return; // caractères imprimables seulement

  const char *glyph = font8x8_basic[(uint8_t)c];

  for (uint8_t row = 0; row < 8; row++) {
    uint8_t bits = glyph[row];
    for (uint8_t col = 0; col < 8; col++) {
      if (bits & (1 << col)) {
          SetPixel4BPP(x + col, y + row, color);
      }
    }
  }
}

void DrawText4BPP(const char* text, uint16_t x, uint16_t y, uint8_t color)
{
  while (*text) {
    DrawChar4BPP(*text, x, y, color);
    x += 8;
    text++;
  }
}

static uint16_t bounding_box_x_min = SSD1320_WIDTH;
static uint16_t bounding_box_x_max = 0;
static uint16_t bounding_box_y_min = SSD1320_HEIGHT;
static uint16_t bounding_box_y_max = 0;

void UpdateBoundingBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (x < bounding_box_x_min) bounding_box_x_min = x;
    if (x + w - 1 > bounding_box_x_max) bounding_box_x_max = x + w - 1;
    if (y < bounding_box_y_min) bounding_box_y_min = y;
    if (y + h - 1 > bounding_box_y_max) bounding_box_y_max = y + h - 1;
}

void DrawRect4BPP(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color, uint8_t filled)
{
  UpdateBoundingBox(x, y, w, h);
  for (uint16_t i = 0; i < h; i++) {
    for (uint16_t j = 0; j < w; j++) {
      if (filled || i == 0 || i == h-1 || j == 0 || j == w-1) {
        SetPixel4BPP(x + j, y + i, color);
      }
    }
  }
}

void ClearRect4BPP(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  UpdateBoundingBox(x, y, w, h);
  for (uint16_t i = 0; i < h; i++) {
    for (uint16_t j = 0; j < w; j++) {
      SetPixel4BPP(x + j, y + i, 0x0); // Efface en mettant la couleur à 0 (noir)
    }
  }
}

void MoveRect4BPP(uint16_t old_x, uint16_t old_y, uint16_t w, uint16_t h, uint16_t new_x, uint16_t new_y, uint8_t color, uint8_t filled) {
  // Effacer l'ancienne position
  ClearRect4BPP(old_x, old_y, w, h);

  // Dessiner à la nouvelle position
  DrawRect4BPP(new_x, new_y, w, h, color, filled);

  // Agrandir la bounding box pour inclure les deux zones
  UpdateBoundingBox(old_x, old_y, w, h);
  UpdateBoundingBox(new_x, new_y, w, h);
}

void ResetBoundingBox() {
    bounding_box_x_min = SSD1320_WIDTH;
    bounding_box_x_max = 0;
    bounding_box_y_min = SSD1320_HEIGHT;
    bounding_box_y_max = 0;
}

void GetBoundingBox(uint16_t *x_min, uint16_t *x_max, uint16_t *y_min, uint16_t *y_max) {
    *x_min = bounding_box_x_min;
    *x_max = bounding_box_x_max;
    *y_min = bounding_box_y_min;
    *y_max = bounding_box_y_max;
}

void DrawLine4BPP(int x0, int y0, int x1, int y1, uint8_t color)
{
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (1) {
    SetPixel4BPP(x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void ClearBuffers() {
  memset(ssd1320_left_buffer, 0x00, SSD1320_BUF_SIZE);
  memset(ssd1320_right_buffer, 0x00, SSD1320_BUF_SIZE);
}

float EaseInOutQuad(float t, float b, float c, float d) {
  t /= d / 2;
  if (t < 1) return c / 2 * t * t + b;
  t--;
  return -c / 2 * (t * (t - 2) - 1) + b;
}
