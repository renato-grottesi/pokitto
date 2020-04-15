#include "DynamicDisplay.hpp"
#include "Pokitto.h"
#include "Synth.h"
#include "World.hpp"
#include "assets.h"
#include "gfxdata.h"
#include "level.h"
#include "pokitto_icon.h"

int main() {
  Pokitto::Core mygame;
  Pokitto::Sound snd;
  DynamicDisplay display;
  World world(display, level_width, level_height, (uint8_t*)level_data);

  int16_t cameraX = 0;
  int16_t cameraY = 256;

  int16_t tonefreq = 33;
  uint8_t amplitude = 255;
  uint8_t wavetype = 1;
  uint8_t arpmode = 0;

  mygame.begin();
  mygame.setFrameRate(100);

  snd.setVolume(snd.getMaxVol());
  snd.ampEnable(1);
  snd.playTone(1, tonefreq, amplitude, wavetype, arpmode);

  // Game loop
  while (mygame.isRunning()) {
    if (mygame.update(true)) {
      world.render();

      const uint16_t incr = 4;
      if (mygame.buttons.rightBtn()) {
        cameraX += incr;
      }
      if (mygame.buttons.leftBtn()) {
        cameraX -= incr;
      }
      if (mygame.buttons.downBtn()) {
        cameraY += incr;
      }
      if (mygame.buttons.upBtn()) {
        cameraY -= incr;
      }
      cameraX = world.updateCameraX(cameraX);
      cameraY = world.updateCameraY(cameraY);

      display.drawSprites();
    }
  }
  return 0;
}
