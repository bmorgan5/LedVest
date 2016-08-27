#include "vest.h"

extern CRGB leds[NUM_LEDS];


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  127

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 255

uint8_t heat[NUM_LEDS] = {0};

#define FLAME_HEIGHT 12

void fire_effect()
{
	for(uint8_t x = 0; x < WIDTH; x++){
		// Step 1.  Cool down every cell a little
		// for(uint8_t y = HEIGHT - 1 ; y >= HEIGHT-FLAME_HEIGHT; y--){
		for(uint8_t f = 0; f < FLAME_HEIGHT; f++){
			uint8_t y = (HEIGHT-1) - f;
			// heat[XY(x,y)] = qsub8(heat[XY(x,y)],  random8(0, 4*(FLAME_HEIGHT-f)));
			heat[XY(x,y)] = qsub8(heat[XY(x,y)],  random8(0, ((COOLING * 10) / (1 + (2*f))) + 2));

		}

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little		
		for( uint8_t y = HEIGHT - 3; y >= HEIGHT-FLAME_HEIGHT; y--) {
			heat[XY(x,y)] = (heat[XY(x,y+1)] + heat[XY(x,y+2)] + heat[XY(x,y+2)]) / 10;
		}

		// Step 3.  Ignite new 'sparks' of heat near the bottom		
		uint8_t y = HEIGHT-1;
		heat[XY(x,y-1)] = qadd8(heat[XY(x,y-1)], random8(0,128));
		heat[XY(x,y)] = qadd8(heat[XY(x,y)], random8(64,192));

		// Step 4.  Map from heat cells to LED colors
		for( uint8_t y = HEIGHT - 1; y >= HEIGHT-FLAME_HEIGHT; y--) {
			CRGB color = HeatColor(heat[XY(x,y)]);			
			leds[XY(x,y)] = color;						
		}
	}
}

#define FLASH_FRAMES 30
struct Flash { int x; int y; int frame; };
void camera_flash()
{
	static Flash frames[FLASH_FRAMES];
	static int frame = 0;

	if( frame == 0)
		memset(frames, 0, sizeof(Flash) * FLASH_FRAMES);

	frames[frame % FLASH_FRAMES].frame = 0;
	switch( frame % 4 ){
		case 0:
			frames[frame % FLASH_FRAMES].x = random16(1, WIDTH/2);
			frames[frame % FLASH_FRAMES].y = random16(1, HEIGHT/2);
			break;
		case 1:
			frames[frame % FLASH_FRAMES].x = random16(WIDTH/2, WIDTH-1);
			frames[frame % FLASH_FRAMES].y = random16(1, HEIGHT/2);
			break;
		case 2:
			frames[frame % FLASH_FRAMES].x = random16(1, WIDTH/2);
			frames[frame % FLASH_FRAMES].y = random16(HEIGHT/2, HEIGHT-1);
			break;
		case 3:
			frames[frame % FLASH_FRAMES].x = random16(WIDTH/2, WIDTH-1);
			frames[frame % FLASH_FRAMES].y = random16(HEIGHT/2, HEIGHT-1);
			break;
	}

	for(int i = 0; i < FLASH_FRAMES; i++) {

		if( frames[i].frame > 7 )
			continue;

		int x = frames[i].x;
		int y = frames[i].y;

		switch(frames[i].frame)
		{
	 		case 0:
				leds[XY(x,y)] = CRGB::Silver;
				break;
	 		case 1:
				leds[XY(x,y)] = CRGB::LightSlateGray;
				leds[XY(x+1,y)] = CRGB::DarkGray;
				leds[XY(x-1,y)] = CRGB::DarkGray;
				leds[XY(x,y-1)] = CRGB::DarkGray;
				leds[XY(x,y+1)] = CRGB::DarkGray;
				break;
	 		case 2:
				leds[XY(x,y)] = CRGB::White;
				leds[XY(x+1,y)] = CRGB::LightSlateGray;
				leds[XY(x-1,y)] = CRGB::LightSlateGray;
				leds[XY(x,y-1)] = CRGB::LightSlateGray;
				leds[XY(x,y+1)] = CRGB::LightSlateGray;
				break;
	 		case 3:
				leds[XY(x,y)] = CRGB::Silver;
				break;
	 		case 4:
				leds[XY(x,y)] = CRGB::Silver;
				break;
	 		case 5:
				leds[XY(x,y)] = CRGB::FairyLight;
				break;
		}
		frames[i].frame++;
	}

	frame++;
}

uint8_t shade_bob_hues[WIDTH][HEIGHT];
// uint8_t shade_bob_hues[HEIGHT][WIDTH];

void set_bob_hue(int8_t x, int8_t y, uint8_t hue)
{
	if(x < 0) {
		x = WIDTH - x;
	}

	if(y < 0){
		y = HEIGHT - y;
	}

	shade_bob_hues[x % WIDTH][y % HEIGHT] = hue;
}

void shade_bob()
{
	static int8_t x = 0;
	static int8_t dx = 1;

	static int8_t y = 0;
	static int8_t dy = 1;

	static int8_t bob_hue = 0;

	static int16_t rainbow_len = 4;

	for(uint32_t c = 0; c < 4; c++){
		set_bob_hue(x+0,y+0,bob_hue);
		set_bob_hue(x+1,y+0,bob_hue);
		set_bob_hue(x+2,y+0,bob_hue);
		set_bob_hue(x+0,y+1,bob_hue);
		set_bob_hue(x+1,y+1,bob_hue);
		set_bob_hue(x+2,y+1,bob_hue);

		// set_bob_hue((x-rainbow_len)+0,(y-rainbow_len)+0,0);
		// set_bob_hue((x-rainbow_len)+1,(y-rainbow_len)+0,0);
		// set_bob_hue((x-rainbow_len)+2,(y-rainbow_len)+0,0);
		// set_bob_hue((x-rainbow_len)+0,(y-rainbow_len)+1,0);
		// set_bob_hue((x-rainbow_len)+1,(y-rainbow_len)+1,0);
		// set_bob_hue((x-rainbow_len)+2,(y-rainbow_len)+1,0);
	 
		x = (x + dx);
		if(x == 0 || x == WIDTH-3)
			dx = -dx;

		y = (y + dy);
		if(y == 0 || y == HEIGHT-2)
			dy = -dy;
		
		bob_hue++;
	}

	
	// x = (x + 1) % (WIDTH - 2);
	// y = (y + 1) % (HEIGHT - 1);

	//TODO: Check array bounds

	for(uint8_t i = 0; i < WIDTH; i++){
		for(uint8_t j = 0; j < HEIGHT; j++){
			if(shade_bob_hues[i][j] % 27 > 13) {
				leds[XY(i,j)] = CHSV(shade_bob_hues[i][j], 255, 128);
			}

		}
	}
}


