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
    x += speeds[is_running];
    if (x > width * TILE_SIZE)
      x = width * TILE_SIZE;
  };
  void BtnL() {
    x -= speeds[is_running];
    if (x < 0)
      x = 0;
  };
  void BtnD() {
    y++;
    if (y > height * TILE_SIZE)
      // GAME OVER!!!
      y = 0;
    if (y > 14 * TILE_SIZE)
      y = 14 * TILE_SIZE;
  };
  void BtnU() {
    y--;
    if (y < 0)
      y = 0;
  };
  void BtnA() { is_running = 1; };
  void BtnB() { is_running = 1; };

  uint16_t cameraX() { return x; }
  uint16_t cameraY() { return y; }

 private:
  DynamicDisplay& display;
  const uint16_t width;
  const uint16_t height;
  uint8_t* data;

  int16_t x = 48;
  int16_t y = 196;
  uint8_t is_running = 0;
  const uint8_t speeds[2] = {2, 4};
};
