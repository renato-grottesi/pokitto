#include "Player.hpp"

#include "assets.h"  // TODO: pass in the constructor

void Player::update(void) {
  Serial pc(USBTX, USBRX);
  if (lastUpdate == 0) {
    lastUpdate = Pokitto::Core::getTime();
    return;
  }

  uint32_t newTime = Pokitto::Core::getTime();
  uint32_t deltaTime = newTime - lastUpdate;
  lastUpdate = newTime;

  const uint8_t stateu8 = static_cast<uint8_t>(state);
  const uint16_t xDrag = 10;
  const uint16_t yGravity = 14;
  const uint32_t speeds[2] = {4000, 7000};

  if (buttons[static_cast<uint8_t>(Button::A)]) {
    is_running = 1;
  }
  if (buttons[static_cast<uint8_t>(Button::B)]) {
    if (state != State::Jump) {
      ySpeed = -5 * speeds[is_running];
      state = State::Jump;
    }
  }
  if (buttons[static_cast<uint8_t>(Button::Right)]) {
    xSpeed = speeds[is_running];
  }
  if (buttons[static_cast<uint8_t>(Button::Left)]) {
    xSpeed = -speeds[is_running];
  }
  if (buttons[static_cast<uint8_t>(Button::Down)]) {
  }
  if (buttons[static_cast<uint8_t>(Button::Up)]) {
  }

  x += xSpeed * deltaTime;
  y += ySpeed * deltaTime;

  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  if ((x >> 16) > width * TILE_SIZE)
    x = (width * TILE_SIZE) << 16;
  // if ((y>>16) > height * TILE_SIZE) // GAME OVER!!!
  if ((y >> 16) > 13 * TILE_SIZE) {
    y = (13 * TILE_SIZE) << 16;
    state = State::Walk;
  }

  if (xSpeed < 0) {
    if ((-xSpeed) > xDrag * deltaTime) {
      xSpeed += xDrag * deltaTime;
    } else {
      xSpeed = 0;
    }
  } else {
    if (xSpeed > xDrag * deltaTime) {
      xSpeed -= xDrag * deltaTime;
    } else {
      xSpeed = 0;
    }
  }

  ySpeed += yGravity * deltaTime;
  if (ySpeed < -10000)
    ySpeed = -10000;
  if (ySpeed > 10000)
    ySpeed = 10000;

  pc.printf("xSpeed = %d, ySpeed = %d, deltaTime = %d\n", xSpeed, ySpeed, deltaTime);

  is_running = 0;

  for (uint8_t b = 0; b < ButtonsCount; b++)
    buttons[b] = 0;
}

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

  return cnt;
}
