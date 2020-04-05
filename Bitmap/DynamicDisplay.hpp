#pragma once

#include "Pokitto.h"

enum class PaletteSize : uint8_t { PAL2, PAL4, PAL16 };

class DynamicSprite {
  friend class DynamicDisplay;

 public:
  DynamicSprite() = default;
  ~DynamicSprite() = default;

  void setup(const uint16_t* _pal,
             const uint8_t* _data,
             uint8_t _transp,
             PaletteSize _palsize,
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
    sy1 = y + data[1];
  }

 private:
  int16_t x;
  int16_t y;
  const uint16_t* pal;
  const uint8_t* data;
  int16_t sx0;
  int16_t sx1;
  int16_t sy1;
  int8_t transp;
  PaletteSize palsize;
};

class DynamicDisplay {
 public:
  DynamicDisplay() = default;
  ~DynamicDisplay() = default;

  void drawLine();
  void drawSprites(uint8_t count);
  uint16_t* getBuffer();

  static const uint8_t maxSprites = 255;
  DynamicSprite sprites[maxSprites];

 private:
#if SHOW_FPS
  uint32_t old_time = 0;
  uint32_t frame_count = 0;
  uint32_t fps = 0;
#endif
};
