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
  void update(void);

  void BtnR() { buttons[static_cast<uint8_t>(Button::Right)] = true; };
  void BtnL() { buttons[static_cast<uint8_t>(Button::Left)] = true; };
  void BtnD() { buttons[static_cast<uint8_t>(Button::Down)] = true; };
  void BtnU() { buttons[static_cast<uint8_t>(Button::Up)] = true; };
  void BtnA() { buttons[static_cast<uint8_t>(Button::A)] = true; };
  void BtnB() { buttons[static_cast<uint8_t>(Button::B)] = true; };

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
  int32_t xSpeed = 0;
  int32_t ySpeed = 0;
  uint8_t is_running = 0;

  enum class Button : uint8_t { Right, Left, Up, Down, A, B, C, Count };
  enum class State : uint8_t { Stand, Walk, Run, Jump, Climb, Count };
  static const uint8_t ButtonsCount = static_cast<uint8_t>(Button::Count);
  static const uint8_t StatesCount = static_cast<uint8_t>(State::Count);
  State state = State::Stand;
  bool buttons[ButtonsCount] = {};
  uint32_t lastUpdate = 0;
};
