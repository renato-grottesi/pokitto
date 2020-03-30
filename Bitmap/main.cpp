#include "Pokitto.h"
#include "gfxdata.h"
#include "pokitto_icon.h"
Pokitto::Core mygame;

static inline void setup_data(uint16_t data)
{
	uint32_t p2=0;

	/** D0...D16 = P2_3 ... P2_18 **/
	p2 = data << 3;

	//__disable_irq();    // Disable Interrupts
	SET_MASK_P2;
	LPC_GPIO_PORT->MPIN[2] = p2; // write bits to port
	CLR_MASK_P2;
	//__enable_irq();     // Enable Interrupts
}

inline void write_command(uint16_t data)
{
	CLR_CS; // select lcd
	CLR_CD; // clear CD = command
	SET_RD; // RD high, do not read
	setup_data(data); // function that inputs the data into the relevant bus lines
	CLR_WR;  // WR low
	SET_WR;  // WR low, then high = write strobe
	SET_CS; // de-select lcd
}

inline void write_data(uint16_t data)
{
	CLR_CS;
	SET_CD;
	SET_RD;
	setup_data(data);
	CLR_WR;
	SET_WR;
	SET_CS;
}

static void prepare(void)
{
	write_command(0x03);
	write_data(0x1038);
	write_command(0x20); // Horizontal DRAM Address
	write_data(0);
	write_command(0x21); // Vertical DRAM Address
	write_data(0);
	write_command(0x22); // write data to DRAM
	CLR_CS_SET_CD_RD_WR;
	SET_MASK_P2;
}

uint32_t __attribute__((section (".bss"))) __attribute__ ((aligned)) screenbuffer[220];

static void colorscreen() {
	// 32 bits of the register with the color:
	// FEDCBA9876543210FEDCBA9876543210
	// ------------RRRRRRGGGGGGBBBBBB--
	// 0x7C000	// red   6 bits
	// 0x03F00	// green 6 bits
	// 0x000FC	// blue  6 bits

	register uint32_t wrbit   __asm("r0") = 1<<12;
	register uint32_t lcd_nil __asm("r1") = 0xA0002188;
	register uint32_t lcd_clr __asm("r2") = 0xA0002284;
	register uint32_t lcd_124 __asm("r3") = 0xA0002204;
	register uint32_t color __asm("r4") = 0;

	//	for(uint8_t y = 0; y < 176; y++) {
	for(uint8_t x = 0; x < 220; x++) {
		color = screenbuffer[x];

		__asm__ __volatile__ (
				"str %[COLOR], [%[LCD]];"    // COLOR at offset 0   of LCD 
				"str %[WRBIT], [%[LCDCLR]];" // WRBIT at offset CLR of LCD 
				"str %[WRBIT], [%[LCD124]];" // WRBIT at offset 124 of LCD
				:
				: [COLOR]  "r" (color),
				[LCD]    "r" (lcd_nil),
				[LCD124] "r" (lcd_124),
				[LCDCLR] "r" (lcd_clr),
				[WRBIT]  "r" (wrbit)
				:
				);
	}
	//	}
}

// MySprite
struct MySprite {
	int16_t x, y, w, h, vx, vy;
};

const int16_t speed = 4;
const uint8_t spriteCount = 4;
const uint8_t spriteW = 32, spriteH = 32;
MySprite mySprites[spriteCount];
uint8_t icon_x = 0;
uint8_t icon_y = 0;

// Palettes
const uint16_t* palettes[spriteCount] = {
	sprite1_pal,
	sprite2_pal,
	sprite3_pal,
	sprite4_pal,
};


int main() {
	// Init
	int16_t startX = 0, startY = 0;
	for (int i = 0; i < spriteCount; i++) {
		// Set my sprite data.
		mySprites[i].x = startX + i * 21;
		mySprites[i].y = startY + i * 21;
		mySprites[i].w = spriteW;
		mySprites[i].h = spriteH;
		mySprites[i].vx = speed;
		mySprites[i].vy = speed;
	}

	// Do not clear the background!
	mygame.display.persistence = 1;

	// Set the palette for the screen buffer
	mygame.display.palette[0] = background_pal[0];
	mygame.display.palette[1] = background_pal[1];
	mygame.display.palette[2] = background_pal[2];
	mygame.display.palette[3] = background_pal[3];

	mygame.begin();

	mygame.setFrameRate(100);  // No limits!
	uint8_t flag = 0;
	uint32_t cshift = 0;

	// Game loop
	while (mygame.isRunning()) {
#if 1
		if (mygame.update(true)) {
			prepare();
			for(uint8_t y = 0; y < 176; y++) {
				for(uint8_t x = 0; x < 220; x++) {
					uint32_t r = (((y+cshift)/8) % 0b111111) << 14;
					uint32_t g = (((x+cshift)/4) % 0b111111) << 8;
					uint32_t b = 0;//((y%4) ? 0xFC: 0);
					screenbuffer[x] = r + g + b;
				}

				if(y>=icon_y && y<icon_y+pokitto_icon[1]) {
					uint16_t bmpy = y-icon_y;
					for(uint8_t x = 0; x < pokitto_icon[0]/2; x++) {
						if(icon_x+x*2 < 220) {
							uint8_t twocolors = pokitto_icon[2 + bmpy*(pokitto_icon[0]/2) + x];
							uint32_t col16_1 = pokitto_icon_pal[((twocolors&0xF0)>>4)&0xF];
							uint32_t col16_2 = pokitto_icon_pal[twocolors&0x0F];
							// Shift from a 565 16 bit to the 32 bit madness
							// FEDCBA9876543210FEDCBA9876543210
							// ------------RRRRRRGGGGGGBBBBBB--
							// ----------------RRRRRGGGGGGBBBBB
							if((((twocolors&0xF0)>>4)&0xF)!=1) screenbuffer[icon_x+x*2+0] = ((col16_1&0xF800) << 4) + ((col16_1&0x07E0) << 3) + ((col16_1&0x001F) << 2);
							if((twocolors&0x0F)!=1)screenbuffer[icon_x+x*2+1] = ((col16_2&0xF800) << 4) + ((col16_2&0x07E0) << 3) + ((col16_2&0x001F) << 2);
						}
					}
				}

				colorscreen();
			}
#else
			if (mygame.update(false)) {
				// Draw the background only in the first rounds.
				mygame.display.setInvisibleColor(-1);
				mygame.display.load565Palette(background_pal);
				mygame.display.drawBitmap(0, 0, background_bmp);
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

					mygame.display.setInvisibleColor(0);
					mygame.display.load565Palette(palettes[i]);
					mygame.display.drawBitmap(x, y, sprite_bmp);

					//
					mySprites[i].x = x;
					mySprites[i].y = y;
				}
#endif
#if 1
				if (mygame.buttons.rightBtn()) { icon_x++; }
				if (mygame.buttons.leftBtn()) { icon_x--; }
				if (icon_x < 1) { icon_x = 1; }
				if (icon_x > 64) { icon_x = 64; }
				if (mygame.buttons.downBtn()) { icon_y++; }
				if (mygame.buttons.upBtn()) { icon_y--; }
				if (icon_y < 1) { icon_y = 1; }
				if (icon_y > 64) { icon_y = 64; }
#endif
			}
		}
		return 0;
	}
