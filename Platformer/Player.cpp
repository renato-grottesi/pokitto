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

  const uint16_t xDrag = 10;
  const uint16_t yGravity = 14;
  const uint32_t speeds[2] = {4000, 7000};

  pc.printf("RLUDABC=%d%d%d%d%d%d%d\n", buttons[0], buttons[1], buttons[2], buttons[3],
            buttons[4], buttons[5], buttons[6]);
  switch (state) {
    case State::Stand: {
      if (buttons[ButtonRight] || buttons[ButtonLeft])
        state = State::Walk;
      else if (buttons[ButtonB]) {
        state = State::Jump;
        jumpStart = newTime;
        jumpSpeed = -5 * speeds[0];
      }
      pc.printf("Stand, speed = %d, %d, delta = %d\n", xSpeed, ySpeed, deltaTime);
      break;
    }
    case State::Walk: {
      if (!buttons[ButtonRight] && !buttons[ButtonLeft])
        state = State::Stand;
      if (buttons[ButtonRight] && buttons[ButtonLeft])
        state = State::Stand;
      else if (buttons[ButtonA])
        state = State::Run;
      else if (buttons[ButtonB]) {
        state = State::Jump;
        jumpStart = newTime;
        jumpSpeed = -5 * speeds[0];
      }
      if (buttons[ButtonRight])
        xSpeed = speeds[0];
      if (buttons[ButtonLeft])
        xSpeed = -speeds[0];
      pc.printf("Walk, speed = %d, %d, delta = %d\n", xSpeed, ySpeed, deltaTime);
      break;
    }
    case State::Run: {
      if (!buttons[ButtonRight] && !buttons[ButtonLeft])
        state = State::Walk;
      if (buttons[ButtonRight] && buttons[ButtonLeft])
        state = State::Walk;
      if (!buttons[ButtonA])
        state = State::Walk;
      if (buttons[ButtonB]) {
        state = State::Jump;
        jumpStart = newTime;
        jumpSpeed = -5 * speeds[1];
      }
      if (buttons[ButtonRight])
        xSpeed = speeds[1];
      if (buttons[ButtonLeft])
        xSpeed = -speeds[1];
      pc.printf("Run, speed = %d, %d, delta = %d\n", xSpeed, ySpeed, deltaTime);
      break;
    }
    case State::Jump: {
      if (buttons[ButtonB] && ((newTime - jumpStart) < 50)) {
        ySpeed = jumpSpeed;
      } else {
        jumpStart = 0;
      }
      if (buttons[ButtonRight])
        xSpeed = speeds[0];
      if (buttons[ButtonLeft])
        xSpeed = -speeds[0];
      if ((y >> 16) > 12 * TILE_SIZE)
        state = State::Stand;  // TODO: replace with touching the floor
      pc.printf("Jump, speed = %d, %d, delta = %d\n", xSpeed, ySpeed, deltaTime);
      break;
    }
    case State::Climb: {
      break;
    }
    default:
      break;
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

  for (uint8_t b = 0; b < ButtonsCount; b++)
    buttons[b] = 0;
  lastUpdate = newTime;
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
