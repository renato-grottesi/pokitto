#pragma once

#include "Pokitto.h"

struct DynamicSprite {
 public:
  int16_t x;
  int16_t y;
  const int16_t w;
  const int16_t h;
  const uint16_t* p;

 private:
  int16_t sx;
  int16_t sy;
  int16_t sx0;
  int16_t sx1;
};

class DynamicDisplay {
 public:
  DynamicDisplay() = default;
  ~DynamicDisplay() = default;

  void startDrawing(void);
  void drawLine();
  uint16_t* getBuffer();

  void drawBitmapPal16(const int16_t screen_y,
                       const int16_t bmp_x,
                       const int16_t bmp_y,
                       const uint16_t* palette,
                       const uint8_t* bitmap,
                       const uint8_t transparent);

  void drawBitmapPal4(const int16_t screen_y,
                      const int16_t bmp_x,
                      const int16_t bmp_y,
                      const uint16_t* palette,
                      const uint8_t* bitmap,
                      const uint8_t transparent);
};
