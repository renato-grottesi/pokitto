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

    // The first x to read shouldn't be negative
    sx0 = x < 0 ? (-x) : 0;
    // The last x to read is assumed to not be negative and it should be inside the screen.
    // The +7 below is to cope with PAL2 bitmaps that need to read 8 colors per pixel.
    sx1 = (x + data[0]) > ((int16_t)LCDWIDTH + 7) ? (((int16_t)LCDWIDTH + 7) - x) : data[0];
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
    // Segment to segment intersection.
    const int16_t a1 = sprites[i].y;
    const int16_t a2 = sprites[i].sy1;

    uint8_t step = LCDHEIGHT / maxSlots;
    uint8_t b1 = 0;
    uint8_t b2 = step;

    for (uint8_t s = 0; s < maxSlots; s++) {
      // TODO: add an overdraw counter.
      if (a2 >= b1 && b2 >= a1 && slotsCnt[s] < maxSpritesPerSlot)
        slots[s][slotsCnt[s]++] = i;
      b1 += step;
      b2 += step;
    }
  }

 private:
  static const uint8_t maxSprites = 255;
  DynamicSprite sprites[maxSprites];
  // Let's assume the geometry is well spread across 11 bands of 176/11=16 pixels each
  static const uint8_t maxSlots = 11;
  // Assuming a tilemap game with 16x16 tiles, there would be max 15*2 tiles in each band.
  // With 64 sprites per band, we still have space for 34/2=17 16x16 sprites in the band.
  static const uint8_t maxSpritesPerSlot = 64;
  uint8_t slots[maxSlots][maxSpritesPerSlot];
  uint8_t slotsCnt[maxSlots] = {
      0,
  };

#if SHOW_FPS
  uint32_t old_time = 0;
  uint32_t frame_count = 0;
  uint32_t fps = 0;
#endif
};
