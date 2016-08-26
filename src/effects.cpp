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
