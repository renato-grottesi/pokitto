#include "OneLineDisplay.hpp"

uint16_t __attribute__((section(".bss"))) __attribute__((aligned))
OneLineDisplay::screenbuffer[LCDWIDTH + 4];

static inline void setup_data(uint16_t data) {
  uint32_t p2 = 0;

  /** D0...D16 = P2_3 ... P2_18 **/
  p2 = data << 3;

  //__disable_irq();    // Disable Interrupts
  SET_MASK_P2;
  LPC_GPIO_PORT->MPIN[2] = p2;  // write bits to port
  CLR_MASK_P2;
  //__enable_irq();     // Enable Interrupts
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

void OneLineDisplay::prepare(void) {
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

void OneLineDisplay::colorscreen() {
  register uint32_t wrbit __asm("r0") = 1 << 12;
  register uint32_t lcd_nil __asm("r1") = 0xA0002188;
  register uint32_t lcd_clr __asm("r2") = 0xA0002284;
  register uint32_t lcd_124 __asm("r3") = 0xA0002204;
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

void OneLineDisplay::drawBitmapPal16(const uint16_t screen_y,
                                     const int16_t bmp_x,
                                     const int16_t bmp_y,
                                     const uint16_t* palette,
                                     const uint8_t* bitmap,
                                     const uint8_t transparent) {
  if (screen_y >= bmp_y && screen_y < bmp_y + bitmap[1]) {
    uint16_t bmpy = screen_y - bmp_y;
    for (uint8_t x = 0; x < bitmap[0] / 2; x++) {
      if (bmp_x + x * 2 < LCDWIDTH) {
        uint8_t twocolors = bitmap[2 + bmpy * (bitmap[0] / 2) + x];
        uint16_t col16_1 = palette[(twocolors >> 4) & 0xF];
        uint16_t col16_2 = palette[(twocolors >> 0) & 0xF];
        if (((twocolors >> 4) & 0xF) != transparent)
          screenbuffer[bmp_x + x * 2 + 0] = col16_1;
        if (((twocolors >> 0) & 0xF) != transparent)
          screenbuffer[bmp_x + x * 2 + 1] = col16_2;
      }
    }
  }
}

void OneLineDisplay::drawBitmapPal4(const uint16_t screen_y,
                                    const int16_t bmp_x,
                                    const int16_t bmp_y,
                                    const uint16_t* palette,
                                    const uint8_t* bitmap,
                                    const uint8_t transparent) {
  if (screen_y >= bmp_y && screen_y < bmp_y + bitmap[1]) {
    uint16_t bmpy = screen_y - bmp_y;
    for (uint8_t x = 0; x < bitmap[0] / 4; x++) {
      if (bmp_x + x * 4 < LCDWIDTH) {
        uint8_t twocolors = bitmap[2 + bmpy * (bitmap[0] / 4) + x];
        uint16_t col16_1 = palette[(twocolors >> 6) & 0x3];
        uint16_t col16_2 = palette[(twocolors >> 4) & 0x3];
        uint16_t col16_3 = palette[(twocolors >> 2) & 0x3];
        uint16_t col16_4 = palette[(twocolors >> 0) & 0x3];
        if (((twocolors >> 6) & 0x3) != transparent)
          screenbuffer[bmp_x + x * 4 + 0] = col16_1;
        if (((twocolors >> 4) & 0x3) != transparent)
          screenbuffer[bmp_x + x * 4 + 1] = col16_2;
        if (((twocolors >> 2) & 0x3) != transparent)
          screenbuffer[bmp_x + x * 4 + 2] = col16_3;
        if (((twocolors >> 0) & 0x3) != transparent)
          screenbuffer[bmp_x + x * 4 + 3] = col16_4;
      }
    }
  }
}
