#define DEBUG 1
#define VISUAL_FPS 0
#include "DynamicDisplay.hpp"

// This is the one line framebuffer of 220 +8 pixels.
// The +8 is to allow 8 pixels of overflow when rendering sprites so that they don't have to
// perform more boundary tests.
static uint16_t __attribute__((section(".bss")))
__attribute__((aligned)) __restrict__ screenbuffer[LCDWIDTH + 8];

static inline void setup_data(uint16_t data) {
  uint32_t p2 = 0;
  /** D0...D16 = P2_3 ... P2_18 **/
  p2 = data << 3;
  SET_MASK_P2;
  LPC_GPIO_PORT->MPIN[2] = p2;  // write bits to port
  CLR_MASK_P2;
}

inline void write_command(uint16_t data) {
  CLR_CS;            // select lcd
  CLR_CD;            // clear CD = command
  SET_RD;            // RD high, do not read
  setup_data(data);  // function that inputs the data into the relevant bus lines
  CLR_WR;            // WR low
  SET_WR;            // WR low, then high = write strobe
  SET_CS;            // de-select lcd
}

inline void write_data(uint16_t data) {
  CLR_CS;
  SET_CD;
  SET_RD;
  setup_data(data);
  CLR_WR;
  SET_WR;
  SET_CS;
}

static void startDrawing(void) {
  write_command(0x03);
  write_data(0x1038);
  write_command(0x20);  // Horizontal DRAM Address
  write_data(0);
  write_command(0x21);  // Vertical DRAM Address
  write_data(0);
  write_command(0x22);  // write data to DRAM
  CLR_CS_SET_CD_RD_WR;
  SET_MASK_P2;
}

/**
 * The method must not be inlined to ensure that all registers
 * are in the r0-r7 range to comply with the thumb mode of
 * the asm block.
 */
void __attribute__((noinline)) DynamicDisplay::drawLine() {
  const register uint32_t wrbit __asm("r0") = 1 << 12;
  const register uint32_t lcd_nil __asm("r1") = 0xA0002188;
  const register uint32_t lcd_clr __asm("r2") = 0xA0002284;
  const register uint32_t lcd_124 __asm("r3") = 0xA0002204;
  register uint32_t color __asm("r4") = 0;

  for (uint8_t x = 0; x < LCDWIDTH; x++) {
    // Shift from a 565 16 bit to the 32 register format
    // FEDCBA9876543210FEDCBA9876543210
    // -------------RRRRRGGGGGGBBBBB---
    // ----------------RRRRRGGGGGGBBBBB
    color = (screenbuffer[x]) << 3;

    __asm__ __volatile__(
        "str %[COLOR], [%[LCD]];"     // COLOR at offset 0   of LCD
        "str %[WRBIT], [%[LCDCLR]];"  // WRBIT at offset CLR of LCD
        "str %[WRBIT], [%[LCD124]];"  // WRBIT at offset 124 of LCD
        :
        : [COLOR] "r"(color), [LCD] "r"(lcd_nil), [LCD124] "r"(lcd_124), [LCDCLR] "r"(lcd_clr),
          [WRBIT] "r"(wrbit)
        :);
  }
}

static void drawBitmapPal16(const int16_t screen_y,
                            const int16_t bmp_x,
                            const int16_t bmp_y,
                            const uint16_t* __restrict__ palette,
                            const uint8_t* __restrict__ bitmap,
                            const uint8_t transparent,
                            const int16_t sx0,
                            const int16_t sx1) {
  const uint32_t width = bitmap[0];
  const uint32_t idxoff = 2 + (screen_y - bmp_y) * (width / 2);
  for (int32_t x = sx0 / 2; x < sx1 / 2; x++) {
    const uint32_t indices = bitmap[idxoff + x];
    const uint8_t shifts[2] = {4, 0};
    for (uint32_t i = 0; i < 2; i++) {
      uint32_t pidx = (indices >> shifts[i]) & 0xF;
      if (pidx != transparent) {
        screenbuffer[bmp_x + x * 2 + i] = palette[pidx];
      }
    }
  }
}

static void drawBitmapPal4(const int16_t screen_y,
                           const int16_t bmp_x,
                           const int16_t bmp_y,
                           const uint16_t* __restrict__ palette,
                           const uint8_t* __restrict__ bitmap,
                           const uint8_t transparent,
                           const int16_t sx0,
                           const int16_t sx1) {
  const uint32_t width = bitmap[0];
  const uint32_t idxoff = 2 + (screen_y - bmp_y) * (width / 4);
  for (int32_t x = sx0 / 4; x < sx1 / 4; x++) {
    const uint32_t indices = bitmap[idxoff + x];
    const uint8_t shifts[4] = {6, 4, 2, 0};
    for (uint32_t i = 0; i < 4; i++) {
      uint32_t pidx = (indices >> shifts[i]) & 0x3;
      if (pidx != transparent) {
        screenbuffer[bmp_x + x * 4 + i] = palette[pidx];
      }
    }
  }
}

static void drawBitmapPal2(const int16_t screen_y,
                           const int16_t bmp_x,
                           const int16_t bmp_y,
                           const uint16_t* __restrict__ palette,
                           const uint8_t* __restrict__ bitmap,
                           const uint8_t transparent,
                           const int16_t sx0,
                           const int16_t sx1) {
  const uint32_t width = bitmap[0];
  const uint32_t idxoff = 2 + (screen_y - bmp_y) * (width / 8);
  for (int32_t x = sx0 / 8; x < sx1 / 8; x++) {
    const uint32_t indices = bitmap[idxoff + x];
    const uint8_t shifts[8] = {7, 6, 5, 4, 3, 2, 1, 0};
    for (uint32_t i = 0; i < 8; i++) {
      uint32_t pidx = (indices >> shifts[i]) & 0x1;
      if (pidx != transparent) {
        screenbuffer[bmp_x + x * 8 + i] = palette[pidx];
      }
    }
  }
}

void (*drawFuncs[])(const int16_t,
                    const int16_t,
                    const int16_t,
                    const uint16_t*,
                    const uint8_t*,
                    const uint8_t,
                    const int16_t,
                    const int16_t) = {&drawBitmapPal2, &drawBitmapPal4, &drawBitmapPal16};

void DynamicDisplay::drawSprites() {
  startDrawing();
#if DEBUG || VISUAL_FPS
  frame_count++;
  uint32_t new_time = Pokitto::Core::getTime();
  if ((new_time - old_time) > 1000) {
    fps = frame_count;
    frame_count = 0;
    old_time = new_time;
    Serial pc(USBTX, USBRX);
    pc.printf("FPS: %d\n", fps);
  }
#endif
  uint8_t y = 0;

  for (uint8_t s = 0; s < maxSlots; s++) {
    for (uint8_t p = 0; p < LCDHEIGHT / maxSlots; p++) {
      for (uint8_t x = 0; x < LCDWIDTH; x++) {
        screenbuffer[x] = topDownColors[y / (LCDHEIGHT / 2)];
      }

      for (uint8_t ii = 0; ii < slotsCnt[s]; ii++) {
        uint8_t i = slots[s][ii];
        if (y < sprites[i].y || y >= sprites[i].sy1)
          continue;
        (drawFuncs[static_cast<uint8_t>(sprites[i].palsize)])(
            y, sprites[i].x, sprites[i].y, sprites[i].pal, sprites[i].data, sprites[i].transp,
            sprites[i].sx0, sprites[i].sx1);
      }

#if VISUAL_FPS
      if (y < 4) {
        for (uint8_t x = 0; x < LCDWIDTH && x < fps; x++) {
          // Let's paint the FPS
          screenbuffer[x] = 0x001f;
        }
      }
      if (y < 2) {
        for (uint8_t x = 0; x < LCDWIDTH; x++) {
          // Let's paint a ruler
          screenbuffer[x] = (x % 10) ? 0x000 : 0xFFFF;
        }
      }
#endif

      drawLine();
      y++;
    }
  }
  for (uint8_t s = 0; s < maxSlots; s++) {
    slotsCnt[s] = 0;
  }
}
void DynamicDisplay::tile(uint8_t i) {
  // Segment to segment intersection.
  const int16_t a1 = sprites[i].y;
  const int16_t a2 = sprites[i].sy1;

  uint8_t step = LCDHEIGHT / maxSlots;
  uint8_t b1 = 0;
  uint8_t b2 = step;

#if DEBUG
  uint32_t overflow = 0;
#endif

  for (uint8_t s = 0; s < maxSlots; s++) {
#if DEBUG
    if (a2 >= b1 && b2 >= a1 && slotsCnt[s] >= maxSpritesPerSlot)
      overflow++;
#endif
    if (a2 >= b1 && b2 >= a1 && slotsCnt[s] < maxSpritesPerSlot)
      slots[s][slotsCnt[s]++] = i;
    b1 += step;
    b2 += step;
  }
#if DEBUG
  if (overflow > 0) {
    Serial pc(USBTX, USBRX);
    pc.printf("%d sprites overflow\n", overflow);
  }
#endif
}
