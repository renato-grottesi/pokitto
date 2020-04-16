#pragma once

#include "DynamicDisplay.hpp"

#define TILE_SIZE 16

class World {
 public:
  World(DynamicDisplay& _display, const uint16_t _width, const uint16_t _height, uint8_t* _data)
      : display(_display), width(_width), height(_height), data(_data) {
    minX = (uint16_t)LCDWIDTH / 2;
    maxX = TILE_SIZE * width - (uint16_t)LCDWIDTH / 2;
    minY = (uint16_t)LCDHEIGHT / 2;
    maxY = TILE_SIZE * height - (uint16_t)LCDHEIGHT / 2;
  };

  ~World() = default;

  uint16_t updateCameraX(uint16_t x) {
    cameraX = x;
    if (x < minX)
      cameraX = minX;
    if (x > maxX)
      cameraX = maxX;
    return cameraX;
  }

  uint16_t updateCameraY(uint16_t y) {
    cameraY = y;
    if (y < minY)
      cameraY = minY;
    if (y > maxY)
      cameraY = maxY;
    return cameraY;
  }

  uint8_t render(uint8_t cnt);

  uint16_t xWorldToTile(uint16_t x) { return x / TILE_SIZE; }
  uint16_t yWorldToTile(uint16_t y) { return y / TILE_SIZE; }
  uint16_t xTileToWorld(uint16_t x) { return x * TILE_SIZE; }
  uint16_t yTileToWorld(uint16_t y) { return y * TILE_SIZE; }

 private:
  DynamicDisplay& display;
  const uint16_t width;
  const uint16_t height;
  uint8_t* data;

  uint16_t minX;
  uint16_t maxX;
  uint16_t minY;
  uint16_t maxY;
  uint16_t cameraX = 0;
  uint16_t cameraY = 0;
};
