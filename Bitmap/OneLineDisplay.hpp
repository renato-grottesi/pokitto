#pragma once

#include "Pokitto.h"

class OneLineDisplay {
 private:
  OneLineDisplay() = default;
  ~OneLineDisplay() = default;

 public:
  static void prepare(void);
  static void colorscreen();

  static void drawBitmapPal16(const uint16_t screen_y,
                              const int16_t bmp_x,
                              const int16_t bmp_y,
                              const uint16_t* palette,
                              const uint8_t* bitmap,
                              const uint8_t transparent);

  static void drawBitmapPal4(const uint16_t screen_y,
                             const int16_t bmp_x,
                             const int16_t bmp_y,
                             const uint16_t* palette,
                             const uint8_t* bitmap,
                             const uint8_t transparent);

 public:
  // This is the one line framebuffer of 220 +4 pixels.
  // The +4 is to allow 4 pixels of overflow when rendering sprites so that they don't have to
  // perform more test.
  static uint16_t __attribute__((section(".bss"))) __attribute__((aligned)) screenbuffer[];
};
