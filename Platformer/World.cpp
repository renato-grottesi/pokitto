#include "World.hpp"
#include "assets.h"  // TODO: pass in the constructor

uint8_t World::render(uint8_t cnt) {
  const uint16_t XCentralTile = xWorldToTile(cameraX);
  const uint16_t YCentralTile = yWorldToTile(cameraY);

  // TODO: clear this parallax code and make it configurable
  const int16_t xOff = (maxX - cameraX) * LCDWIDTH / maxX;
  const int16_t yOff = (maxY - cameraY) * LCDHEIGHT / maxY / 5;
  display.setup(cnt++, bg_001_pal, bg_001_data, 0xf, PaletteSize::PAL2, xOff - bg_001_data[0],
                LCDHEIGHT / 2 - bg_001_data[1] / 2 + yOff);
  display.setup(cnt++, bg_001_pal, bg_001_data, 0xf, PaletteSize::PAL2, xOff,
                LCDHEIGHT / 2 - bg_001_data[1] / 2 + yOff);

  for (int16_t yt = YCentralTile - 6; yt < YCentralTile + 7; yt++) {
    for (int16_t xt = XCentralTile - 7; xt < XCentralTile + 8; xt++) {
      if (xt < 0)
        continue;
      if (yt < 0)
        continue;
      if (xt >= width)
        continue;
      if (yt >= height)
        continue;
      uint8_t tile = data[yt * width + xt];
      if (tile) {
        tile--;
        uint16_t screenX = minX + xTileToWorld(xt) - cameraX;
        uint16_t screenY = minY + yTileToWorld(yt) - cameraY;
        display.setup(cnt++, tile_pals[tile], tile_datas[tile],
                      tile_pals[tile][0] == 0 ? 0x00 : 0xff, palSizes[tile], screenX, screenY);
      }
    }
  }

  return cnt;
}
