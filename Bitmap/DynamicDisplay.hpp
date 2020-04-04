#pragma once

#include "Pokitto.h"

class DynamicSprite {
 public:
  DynamicSprite() = default;
  ~DynamicSprite() = default;

  void setup(const uint16_t* _pal,
             const uint8_t* _data,
             uint8_t _transp,
             uint8_t _palsize,
             int16_t _x,
             int16_t _y) {
    pal = _pal;
    data = _data;
    transp = _transp;
    palsize = _palsize;
    setPosition(_x, _y);
  };

  void setPosition(int16_t _x, int16_t _y) {
    x = _x;
    y = _y;

    sx0 = x < 0 ? (-x) : 0;
    sx1 = (x + data[0]) > ((int16_t)LCDWIDTH) ? (((int16_t)LCDWIDTH) - x) : data[0];
  }

 public:
  int16_t x;
  int16_t y;
  const uint16_t* pal;
  const uint8_t* data;
  int16_t sx0;
  int16_t sx1;
  int8_t transp;
  int8_t palsize;
};

class DynamicDisplay {
 public:
  DynamicDisplay() = default;
  ~DynamicDisplay() = default;

  void startDrawing(void);
  void drawLine();
  void drawSprites(uint8_t count);
  uint16_t* getBuffer();

  void drawBitmapPal16(const int16_t screen_y,
                       const int16_t bmp_x,
                       const int16_t bmp_y,
                       const uint16_t* palette,
                       const uint8_t* bitmap,
                       const uint8_t transparent,
                       const int16_t sx0,
                       const int16_t sx1);

  void drawBitmapPal4(const int16_t screen_y,
                      const int16_t bmp_x,
                      const int16_t bmp_y,
                      const uint16_t* palette,
                      const uint8_t* bitmap,
                      const uint8_t transparent,
                      const int16_t sx0,
                      const int16_t sx1);
  static const uint8_t maxSprites = 255;
  DynamicSprite sprites[maxSprites];
#if SHOW_FPS
  uint32_t old_time = 0;
  uint32_t frame_count = 0;
  uint32_t fps = 0;
#endif
};
