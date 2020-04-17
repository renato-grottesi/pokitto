#pragma once

#include "DynamicDisplay.hpp"

#define TILE_SIZE 16

class Player {
 public:
  Player(DynamicDisplay& _display,
         const uint16_t _width,
         const uint16_t _height,
         uint8_t* _data)
      : display(_display), width(_width), height(_height), data(_data){};
  ~Player() = default;

  uint8_t render(uint8_t cnt, int16_t camX, int16_t camY);

  void BtnR() {
    xSpeed = speeds[is_running];
    xDir = 1;
  };
  void BtnL() {
    xSpeed = speeds[is_running];
    xDir = -1;
  };
  void BtnD() {
    ySpeed = 30000;
    yDir = 1;
  };
  void BtnU() {
    ySpeed = -30000;
    yDir = -1;
  };
  void BtnA() { is_running = 1; };
  void BtnB() { ySpeed = -400000; };

  uint16_t cameraX() { return x >> 16; }
  uint16_t cameraY() { return y >> 16; }

 private:
  DynamicDisplay& display;
  const uint16_t width;
  const uint16_t height;
  uint8_t* data;

  // x and y are int16_t with 16 bits of decimal precision.
  int32_t x = 48 << 16;
  int32_t y = 196 << 16;
  const uint32_t speeds[2] = {90000, 150000};
  uint16_t xDrag = 10000;
  uint16_t yDrag = 10000;
  int32_t xSpeed = 0;
  int8_t xDir = 1;
  int32_t ySpeed = 0;
  int8_t yDir = 1;
  uint8_t is_running = 0;
};
