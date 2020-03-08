#include "Pokitto.h"
#include "pokitto_icon.h"

Pokitto::Core mygame;

int main() {
  mygame.begin();
  mygame.display.load565Palette(pokitto_icon_pal);
  mygame.display.bgcolor = 0;

  int16_t x = 0;
  int16_t y = 0;
  while (mygame.isRunning()) {
    if (mygame.update()) {
      if (mygame.buttons.rightBtn())
        x++;
      if (mygame.buttons.leftBtn())
        x--;
      if (mygame.buttons.downBtn())
        y++;
      if (mygame.buttons.upBtn())
        y--;
      if (x < 0)
        x = 0;
      if (x > 64)
        x = 128;
      if (y < 0)
        y = 0;
      if (y > 64)
        y = 128;
      mygame.display.drawBitmap(x, y, pokitto_icon);
    }
  }

  return 0;
}
