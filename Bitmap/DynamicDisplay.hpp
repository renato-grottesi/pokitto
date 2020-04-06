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
  };

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
  void drawSprites();
  uint16_t* getBuffer();

  void setup(uint8_t _i,
             const uint16_t* _pal,
             const uint8_t* _data,
             uint8_t _transp,
             PaletteSize _palsize,
             int16_t _x,
             int16_t _y) {
    sprites[_i].setup(_pal, _data, _transp, _palsize, _x, _y);
    tile(_i);
  };

  void setPosition(uint8_t _i, int16_t _x, int16_t _y) {
    sprites[_i].setPosition(_x, _y);
    tile(_i);
  };

 private:
  void tile(uint8_t i) {
    const int16_t x1 = sprites[i].y;
    const int16_t x2 = sprites[i].sy1;

    uint8_t step = LCDHEIGHT / maxSlots;
    uint8_t y1 = 0;
    uint8_t y2 = step;

    for (uint8_t s = 0; s < maxSlots; s++) {
      if (x2 >= y1 && y2 >= x1)
        slots[s][slotsCnt[s]++] = i;
      y1 += step;
      y2 += step;
    }
  }

 private:
  static const uint8_t maxSprites = 255;
  DynamicSprite sprites[maxSprites];
  static const uint8_t maxSlots = 8;  // it looks like 8 is the best and it takes 2k of RAM
  uint8_t slots[maxSlots][256];
  uint8_t slotsCnt[maxSlots] = {
      0,
  };

#if SHOW_FPS
  uint32_t old_time = 0;
  uint32_t frame_count = 0;
  uint32_t fps = 0;
#endif
};
