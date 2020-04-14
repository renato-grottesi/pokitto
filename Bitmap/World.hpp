#include "DynamicDisplay.hpp"

class World {
 public:
  World(const DynamicDisplay& _display,
        const uint16_t _height,
        const uint16_t _width,
        uint8_t** _data)
      : height(_height), width(_width), data(_data), display(_display) {
    minX = (uint16_t)LCDWIDTH / 2;
    maxX = 16 * width - (uint16_t)LCDWIDTH / 2;
    minY = (uint16_t)LCDHEIGHT / 2;
    maxY = 16 * height - (uint16_t)LCDHEIGHT / 2;
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
  void render();

 private:
  const uint16_t height;
  const uint16_t width;
  uint16_t minX;
  uint16_t maxX;
  uint16_t minY;
  uint16_t maxY;
  uint16_t cameraX = 0;
  uint16_t cameraY = 0;
  uint8_t** data;
  const DynamicDisplay& display;
};
