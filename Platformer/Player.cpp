#include "Player.hpp"

#include "assets.h"  // TODO: pass in the constructor

uint8_t Player::render(uint8_t cnt, int16_t camX, int16_t camY) {
  uint8_t tile = 60;

  int16_t cx = (int16_t)(LCDWIDTH / 2) - camX + x;
  int16_t cy = (int16_t)(LCDHEIGHT / 2) - camY + y;

  display.setup(cnt++, tile_pals[tile], tile_datas[tile], tile_pals[tile][0] == 0 ? 0x00 : 0xff,
                palSizes[tile], cx - 8, cy - 8);

  // When not crunching, draw the tip of the head.
  tile = 58;
  display.setup(cnt++, tile_pals[tile], tile_datas[tile], tile_pals[tile][0] == 0 ? 0x00 : 0xff,
                palSizes[tile], cx - 8, cy - 8 - 16);

  is_running = 0;

  return cnt;
}
