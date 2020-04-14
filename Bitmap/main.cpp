#include "DynamicDisplay.hpp"
#include "Pokitto.h"
#include "Synth.h"
#include "World.hpp"
#include "assets.h"
#include "gfxdata.h"
#include "level.h"
#include "pokitto_icon.h"

struct MySprite {
  int16_t x, y, w, h, vx, vy;
  const uint16_t* pal;
};

int main() {
  Pokitto::Core mygame;
  Pokitto::Sound snd;
  DynamicDisplay display;
  World world(display, level_width, level_height, (uint8_t**)level_data);

  const int16_t speed = 4;
  const uint8_t spriteCount = 255;
  MySprite mySprites[spriteCount];
  int16_t icon_x = 0;
  int16_t icon_y = 0;

  int16_t tonefreq = 33;
  uint8_t amplitude = 255;
  uint8_t wavetype = 1;
  uint8_t arpmode = 0;

  // Drawing a full screen image has quite an impact on FPS
  // display.setup(0, background_pal, background_bmp, 0, PaletteSize::PAL4, 0, 0);
  display.setup(1, pokitto_icon_pal, pokitto_icon, 1, PaletteSize::PAL16, 0, 0);
  // display.setup(0, rgb565pal, tile_152, 0, PaletteSize::PAL4, 0, 0);
  display.setup(0, bg_001_pal, bg_001_data, 0xf, PaletteSize::PAL2, 0, 0);
  // display.setup(1, tile_025_pal, tile_025_data, 0x0, PaletteSize::PAL16, 0, 0);

  srand(134719);
  const uint16_t* pals[] = {sprite1_pal, sprite2_pal, sprite3_pal, sprite4_pal};
  //  const uint8_t* bmps[] = {sprite_bmp, sprite_plus};
  const uint8_t* bmps[] = {sprite_plus_16, sprite_plus_16};
  //  const uint8_t* bmps[] = {sprite_plus_8, sprite_plus_8};
  for (int i = 2; i < spriteCount; i++) {
    mySprites[i].x = 16 + rand() % 128;
    mySprites[i].y = 16 + rand() % 64;
    mySprites[i].w = sprite_bmp[0];
    mySprites[i].h = sprite_bmp[1];
    mySprites[i].vx = -8 + rand() % 16;
    mySprites[i].vy = -8 + rand() % 16;
    // mySprites[i].pal = pals[rand() % 4];
    // display.setup(i, pals[rand() % 4], bmps[rand() % 2], 0, PaletteSize::PAL4,
    // mySprites[i].x, mySprites[i].y);
    mySprites[i].pal = tile_pals[i % 180];
    display.setup(i, tile_pals[i % 180], tile_datas[i % 180],
                  tile_pals[i % 180][0] == 0 ? 0x00 : 0xff, palSizes[i % 180], mySprites[i].x,
                  mySprites[i].y);
  }

  mygame.begin();
  mygame.setFrameRate(100);

  snd.setVolume(snd.getMaxVol());
  snd.ampEnable(1);
  snd.playTone(1, tonefreq, amplitude, wavetype, arpmode);

  // Game loop
  while (mygame.isRunning()) {
    if (mygame.update(true)) {
      display.drawSprites();
      icon_x = world.updateCameraX(icon_x);
      icon_y = world.updateCameraY(icon_y);
      world.render();

      display.setPosition(0, 0, 0);
      // Move mySprites advancing x and y and bouncing from hidden edges
      if (mygame.buttons.aBtn()) {
        for (int i = 2; i < spriteCount; i++) {
          display.setPosition(i, (i % 14) * 16, (i / 11) * 16);
        }
      } else {
        for (int i = 2; i < spriteCount; i++) {
          int16_t x = mySprites[i].x, y = mySprites[i].y;
          int16_t w = mySprites[i].w, h = mySprites[i].h;

          x += mySprites[i].vx;
          y += mySprites[i].vy;

          if (x < -mySprites[i].w) {
            x = -mySprites[i].w;
            mySprites[i].vx = speed;
          } else if (x >= mygame.display.getWidth() - w + mySprites[i].w) {
            x = mygame.display.getWidth() - 1 - w + mySprites[i].w;
            mySprites[i].vx = -speed;
          }
          if (y < -mySprites[i].h) {
            y = -mySprites[i].h;
            mySprites[i].vy = speed;
          } else if (y >= mygame.display.getHeight() - h + mySprites[i].h) {
            y = mygame.display.getHeight() - 1 - h + mySprites[i].h;
            mySprites[i].vy = -speed;
          }

          mySprites[i].x = x;
          mySprites[i].y = y;
          display.setPosition(i, x, y);
        }
      }

      // Move pokitto icon by buttons input
      if (mygame.buttons.rightBtn()) {
        icon_x += 2;
      }
      if (mygame.buttons.leftBtn()) {
        icon_x -= 2;
      }
      if (icon_x < -pokitto_icon[0] / 2) {
        icon_x = -pokitto_icon[0] / 2;
      }
      if (icon_x > int16_t(LCDWIDTH - pokitto_icon[0] / 2)) {
        icon_x = int16_t(LCDWIDTH - pokitto_icon[0] / 2);
      }
      if (mygame.buttons.downBtn()) {
        icon_y += 2;
      }
      if (mygame.buttons.upBtn()) {
        icon_y -= 2;
      }
      if (icon_y < -pokitto_icon[1] / 2) {
        icon_y = -pokitto_icon[1] / 2;
      }
      if (icon_y > int16_t(LCDHEIGHT - pokitto_icon[1] / 2)) {
        icon_y = int16_t(LCDHEIGHT - pokitto_icon[1] / 2);
      }
      display.setPosition(1, icon_x, icon_y);
    }
  }
  return 0;
}
