#include "Player.hpp"

#include "assets.h"  // TODO: pass in the constructor

static const uint8_t COLL_R = 0b00000001;
static const uint8_t COLL_L = 0b00000010;
static const uint8_t COLL_U = 0b00000100;
static const uint8_t COLL_D = 0b00001000;

uint8_t Player::collide() {
  static const uint8_t htb = 116;  // hard tiles begin
  int32_t ptx = (x >> 16) / TILE_SIZE;
  int32_t pty = (y >> 16) / TILE_SIZE;

  uint8_t collRes = 0;
  uint8_t tile;
  int32_t distance;

  if (ptx <= 0) {
    collRes |= COLL_L;
  }
  if (ptx >= width) {
    collRes |= COLL_R;
  }
  if (pty <= 0) {
    collRes |= COLL_U;
  }
  if (pty >= height) {
    collRes |= COLL_D;
  }

  if (ptx > 0)
    ptx--;
  tile = data[pty * width + ptx];
  distance = (x >> 16) - 8 - (ptx * TILE_SIZE);
  distance *= distance;
  if (distance < (TILE_SIZE * TILE_SIZE)) {
    if (tile > htb)
      collRes |= COLL_L;
    if (tile == 13)
      data[pty * width + ptx] = 0;
  }

  ptx++;
  if (pty > 0)
    pty--;
  tile = data[pty * width + ptx];
  distance = (y >> 16) - 8 - (pty * TILE_SIZE);
  distance *= distance;
  if (distance < (TILE_SIZE * TILE_SIZE)) {
    if (tile > htb)
      collRes |= COLL_U;
    if (tile == 13)
      data[pty * width + ptx] = 0;
  }

  pty++;
  pty++;
  tile = data[pty * width + ptx];
  distance = (y >> 16) - 8 - (pty * TILE_SIZE);
  distance *= distance;
  if (distance < (TILE_SIZE * TILE_SIZE)) {
    if (tile > htb)
      collRes |= COLL_D;
    if (tile == 13)
      data[pty * width + ptx] = 0;
  }

  pty--;
  ptx++;
  tile = data[pty * width + ptx];
  distance = (x >> 16) - 8 - (ptx * TILE_SIZE);
  distance *= distance;
  if (distance < (TILE_SIZE * TILE_SIZE)) {
    if (tile > htb)
      collRes |= COLL_R;
    if (tile == 13)
      data[pty * width + ptx] = 0;
  }

  return collRes;
}

void Player::update(void) {
  Serial pc(USBTX, USBRX);
  if (lastUpdate == 0) {
    lastUpdate = Pokitto::Core::getTime();
    return;
  }

  uint32_t newTime = Pokitto::Core::getTime();
  uint32_t deltaTime = newTime - lastUpdate;

  const uint16_t xDrag = 10;
  const uint16_t yGravity = 24;
  const uint32_t speeds[2] = {4000, 7000};
  const int32_t jSpeeds[2] = {-7000, -9000};

  pc.printf("RLUD=%d%d%d%d\n", (collision & COLL_R) != 0, (collision & COLL_L) != 0,
            (collision & COLL_U) != 0, (collision & COLL_D) != 0);

  // pc.printf("RLUDABC=%d%d%d%d%d%d%d\n", buttons[0], buttons[1], buttons[2], buttons[3],
  //          buttons[4], buttons[5], buttons[6]);
  switch (state) {
    case State::Stand: {
      if (buttons[ButtonRight] || buttons[ButtonLeft])
        state = State::Walk;
      else if (buttons[ButtonB]) {
        state = State::Jump;
        jumpStart = newTime;
        jumpSpeed = jSpeeds[0];
      }
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
        jumpSpeed = jSpeeds[0];
      }
      if (buttons[ButtonRight])
        xSpeed = speeds[0];
      if (buttons[ButtonLeft])
        xSpeed = -speeds[0];
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
        jumpSpeed = jSpeeds[1];
      }
      if (buttons[ButtonRight])
        xSpeed = speeds[1];
      if (buttons[ButtonLeft])
        xSpeed = -speeds[1];
      break;
    }
    case State::Jump: {
      if (buttons[ButtonB] && ((newTime - jumpStart) < 250)) {
        ySpeed = jumpSpeed;
      } else {
        jumpStart = 0;
      }
      if (buttons[ButtonRight])
        xSpeed = speeds[0];
      if (buttons[ButtonLeft])
        xSpeed = -speeds[0];
      if (collision & COLL_D || (y >> 16) > 12 * TILE_SIZE) {
        state = State::Stand;
      }
      break;
    }
    case State::Climb: {
      break;
    }
    default:
      break;
  }

  int32_t old_x = x;
  int32_t old_y = y;

  // Update position and limit the increase to max 4 pixel to avoid collision glitches
  if ((xSpeed > 0 && !(collision & COLL_R)) || (xSpeed < 0 && !(collision & COLL_L))) {
    int32_t inc = xSpeed > 0 ? xSpeed * deltaTime : -xSpeed * deltaTime;
    if (inc > (4 << 16))
      inc = 4 << 16;
    x += inc * (xSpeed > 0 ? 1 : -1);
  }
  if (!(collision & COLL_D) || (ySpeed < 0)) {
    int32_t inc = ySpeed > 0 ? ySpeed * deltaTime : -ySpeed * deltaTime;
    if (inc > (4 << 16))
      inc = 4 << 16;
    y += inc * (ySpeed > 0 ? 1 : -1);
  } else
    ySpeed = 0;

  collision = collide();
  if (collision & (COLL_R | COLL_L))
    x = old_x;
  if (collision & (COLL_U | COLL_D))
    y = old_y;

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
  if (ySpeed < -100000)
    ySpeed = -100000;
  if (ySpeed > 100000)
    ySpeed = 100000;

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
