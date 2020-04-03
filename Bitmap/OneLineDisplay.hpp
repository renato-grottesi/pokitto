#pragma once

#include "Pokitto.h"

class OneLineDisplay {
 public:
  OneLineDisplay() = default;
  ~OneLineDisplay() = default;

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
