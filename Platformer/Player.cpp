#include "Player.hpp"

#include "assets.h"  // TODO: pass in the constructor

uint8_t Player::render(uint8_t cnt, int16_t camX, int16_t camY) {
  uint8_t tile = 60;

  int16_t cx = (int16_t)(LCDWIDTH / 2) - camX + (x >> 16);
  int16_t cy = (int16_t)(LCDHEIGHT / 2) - camY + (y >> 16);

  display.setup(cnt++, tile_pals[tile], tile_datas[tile], tile_pals[tile][0] == 0 ? 0x00 : 0xff,
                palSizes[tile], cx - 8, cy - 8);

  // When not crunching, draw the tip of the head.
  tile = 58;
  display.setup(cnt++, tile_pals[tile], tile_datas[tile], tile_pals[tile][0] == 0 ? 0x00 : 0xff,
                palSizes[tile], cx - 8, cy - 8 - 16);

  x += xSpeed * xDir;
  y += ySpeed * yDir;
  y += yDrag;

  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  if ((x >> 16) > width * TILE_SIZE)
    x = (width * TILE_SIZE) << 16;
  // if ((y>>16) > height * TILE_SIZE) // GAME OVER!!!
  if ((y >> 16) > 13 * TILE_SIZE)
    y = (13 * TILE_SIZE) << 16;

  xSpeed -= xDrag;
  ySpeed -= yDrag;

  if (xSpeed < 0)
    xSpeed = 0;
  if (ySpeed < 0)
    ySpeed = 0;

  is_running = 0;

  return cnt;
}
