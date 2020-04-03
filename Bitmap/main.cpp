#include "OneLineDisplay.hpp"
#include "Pokitto.h"
#include "Synth.h"
#include "gfxdata.h"
#include "pokitto_icon.h"

#define SHOW_FPS 1

Pokitto::Core mygame;
Pokitto::Sound snd;
OneLineDisplay display;

// MySprite
struct MySprite {
  int16_t x, y, w, h, vx, vy;
  const uint16_t* pal;
};

const int16_t speed = 4;
const uint8_t spriteCount = 4;
const uint8_t spriteW = 32, spriteH = 32;
MySprite mySprites[spriteCount];
int16_t icon_x = 0;
int16_t icon_y = 0;

int16_t tonefreq = 33;
uint8_t amplitude = 255;
uint8_t wavetype = 1;
uint8_t arpmode = 0;

int main() {
  int16_t startX = 0, startY = 0;
  for (int i = 0; i < spriteCount; i++) {
    mySprites[i].x = startX + i * 21;
    mySprites[i].y = startY + i * 21;
    mySprites[i].w = spriteW;
    mySprites[i].h = spriteH;
    mySprites[i].vx = speed;
    mySprites[i].vy = speed;
  }
  mySprites[0].pal = sprite1_pal;
  mySprites[1].pal = sprite2_pal;
  mySprites[2].pal = sprite3_pal;
  mySprites[3].pal = sprite4_pal;

  mygame.begin();
  mygame.setFrameRate(100);

  snd.setVolume(snd.getMaxVol());
  snd.ampEnable(1);
  snd.playTone(1, tonefreq, amplitude, wavetype, arpmode);

  uint16_t* screenbuffer = display.getBuffer();

#if SHOW_FPS
  uint32_t old_time = mygame.getTime();
  uint32_t frame_count = 0;
  uint32_t fps = 0;
#endif

  // Game loop
  while (mygame.isRunning()) {
    if (mygame.update(true)) {
#if SHOW_FPS
      frame_count++;
      uint32_t new_time = mygame.getTime();
      if ((new_time - old_time) > 1000) {
        fps = frame_count;
        frame_count = 0;
        old_time = new_time;
      }
#endif

      display.startDrawing();
      for (uint8_t y = 0; y < LCDHEIGHT; y++) {
        for (uint8_t x = 0; x < LCDWIDTH; x++) {
          // Let's setup a nice sunset with code
          screenbuffer[x] = (y / 6) << 0;
        }

        // Drawing a full screen image has quite an impact on FPS: 50->30
        display.drawBitmapPal4(y, 0, 0, background_pal, background_bmp, 0);

        display.drawBitmapPal16(y, icon_x, icon_y, pokitto_icon_pal, pokitto_icon, 1);

        for (int i = 0; i < spriteCount; i++) {
          display.drawBitmapPal4(y, mySprites[i].x, mySprites[i].y, mySprites[i].pal,
                                 sprite_bmp, 0);
        }

#if SHOW_FPS
        if (y < 4) {
          for (uint8_t x = 0; x < LCDWIDTH && x < fps; x++) {
            // Let's paint the FPS
            screenbuffer[x] = 0x001f;
          }
        }
        if (y < 2) {
          for (uint8_t x = 0; x < LCDWIDTH; x++) {
            // Let's paint a ruler
            screenbuffer[x] = (x % 10) ? 0x000 : 0xFFFF;
          }
        }
#endif

        display.drawLine();
      }

      // Move mySprites
      for (int i = 0; i < spriteCount; i++) {
        int16_t x = mySprites[i].x, y = mySprites[i].y;
        int16_t w = mySprites[i].w, h = mySprites[i].h;

        // Advance x and y.
        x += mySprites[i].vx;
        y += mySprites[i].vy;

        // Bounce from hidden edges
        if (x < -spriteW) {
          x = -spriteW;
          mySprites[i].vx = speed;
        } else if (x >= mygame.display.getWidth() - w + spriteW) {
          x = mygame.display.getWidth() - 1 - w + spriteW;
          mySprites[i].vx = -speed;
        }
        if (y < -spriteH) {
          y = -spriteH;
          mySprites[i].vy = speed;
        } else if (y >= mygame.display.getHeight() - h + spriteH) {
          y = mygame.display.getHeight() - 1 - h + spriteH;
          mySprites[i].vy = -speed;
        }

        mySprites[i].x = x;
        mySprites[i].y = y;
      }

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
    }
  }
  return 0;
}
