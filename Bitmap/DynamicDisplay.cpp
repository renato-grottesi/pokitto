#include "DynamicDisplay.hpp"

// This is the one line framebuffer of 220 +4 pixels.
// The +4 is to allow 4 pixels of overflow when rendering sprites so that they don't have to
// perform more boundary tests.
static uint16_t __attribute__((section(".bss"))) __attribute__((aligned))
screenbuffer[LCDWIDTH + 8];

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

void DynamicDisplay::startDrawing(void) {
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

void DynamicDisplay::drawLine() {
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

void DynamicDisplay::drawBitmapPal16(const int16_t screen_y,
                                     const int16_t bmp_x,
                                     const int16_t bmp_y,
                                     const uint16_t* palette,
                                     const uint8_t* bitmap,
                                     const uint8_t transparent) {
  const uint32_t width = bitmap[0];
  const uint32_t height = bitmap[1];
  const uint32_t idxoff = 2 + (screen_y - bmp_y) * (width / 2);
  const uint32_t x0 = bmp_x < 0 ? (-bmp_x) / 2 : 0;
  const uint32_t x1 =
      (bmp_x + width) > ((int16_t)LCDWIDTH) ? (((int16_t)LCDWIDTH) - bmp_x) / 2 : width / 2;
  if (screen_y < bmp_y)
    return;
  if (screen_y >= (bmp_y + height))
    return;
  for (int32_t x = x0; x < x1; x++) {
    uint32_t indices = bitmap[idxoff + x];
    uint8_t shifts[2] = {4, 0};
    for (uint32_t i = 0; i < 2; i++) {
      uint32_t pidx = (indices >> shifts[i]) & 0xF;
      if (pidx != transparent) {
        screenbuffer[bmp_x + x * 2 + i] = palette[pidx];
      }
    }
  }
}

void DynamicDisplay::drawBitmapPal4(const int16_t screen_y,
                                    const int16_t bmp_x,
                                    const int16_t bmp_y,
                                    const uint16_t* palette,
                                    const uint8_t* bitmap,
                                    const uint8_t transparent) {
  const uint32_t width = bitmap[0];
  const uint32_t height = bitmap[1];
  const register uint32_t idxoff __asm("r0") = 2 + (screen_y - bmp_y) * (width / 4);
  const uint32_t x0 = bmp_x < 0 ? (-bmp_x) / 4 : 0;
  const uint32_t x1 =
      (bmp_x + width) > ((int16_t)LCDWIDTH) ? (((int16_t)LCDWIDTH) - bmp_x) / 4 : width / 4;
  if (screen_y < bmp_y)
    return;
  if (screen_y >= (bmp_y + height))
    return;
  for (int32_t x = x0; x < x1; x++) {
    uint32_t indices = bitmap[idxoff + x];
    uint8_t shifts[4] = {6, 4, 2, 0};
    for (uint32_t i = 0; i < 4; i++) {
      uint32_t pidx = (indices >> shifts[i]) & 0x3;
      if (pidx != transparent) {
        screenbuffer[bmp_x + x * 4 + i] = palette[pidx];
      }
    }
  }
}

uint16_t* DynamicDisplay::getBuffer() {
  return screenbuffer;
}
