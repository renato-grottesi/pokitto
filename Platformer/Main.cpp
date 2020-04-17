#include "DynamicDisplay.hpp"
#include "Player.hpp"
#include "Pokitto.h"
#include "Synth.h"
#include "World.hpp"
#include "assets.h"
#include "level.h"

int main() {
  Pokitto::Core mygame;
  Pokitto::Sound snd;
  DynamicDisplay display;
  World world(display, level_width, level_height, (uint8_t*)level_data);
  Player player(display, level_width, level_height, (uint8_t*)level_data);
  display.setTopDownColors(0x6DE0, 0x4400);

  int16_t cameraX = 0;
  int16_t cameraY = 256;

  int16_t tonefreq = 13;
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
      if (mygame.buttons.aBtn()) {
        player.BtnA();
      }
      if (mygame.buttons.bBtn()) {
        player.BtnB();
      }
      if (mygame.buttons.rightBtn()) {
        player.BtnR();
      }
      if (mygame.buttons.leftBtn()) {
        player.BtnL();
      }
      if (mygame.buttons.downBtn()) {
        player.BtnD();
      }
      if (mygame.buttons.upBtn()) {
        player.BtnU();
      }

      player.update();

      cameraX = world.updateCameraX(player.cameraX());
      cameraY = world.updateCameraY(player.cameraY());

      uint8_t cnt = 0;
      cnt = world.render(cnt);
      cnt = player.render(cnt, cameraX, cameraY);
      display.drawSprites();
    }
  }
  return 0;
}
