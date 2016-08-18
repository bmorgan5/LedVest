#include "vest.h"

extern CRGB leds[NUM_LEDS];

static void show_rgb(){
	for(uint8_t j = 0; j < 8; j++){
		for(uint8_t i = 0; i < WIDTH; i++){
			leds[XY(i,j)] = CRGB::Red;
		}	
	}

	for(uint8_t j = 8; j < 16; j++){
		for(uint8_t i = 0; i < WIDTH; i++){
			leds[XY(i,j)] = CRGB::Green;
		}	
	}

	for(uint8_t j = 16; j < HEIGHT; j++){
		for(uint8_t i = 0; i < WIDTH; i++){
			leds[XY(i,j)] = CRGB::Blue;
		}	
	}
}

static void add_sparkles() {
	for(uint8_t i = 0; i < 30; i++){
		uint16_t index = XY(random16(WIDTH), random16(HEIGHT));

		if(leds[index] == CRGB(0,0,0)) {
			leds[index] = CRGB::White;
		}
	}
}

void test_sparkles(){
	FastLED.clear(1);
	// for(uint16_t i = 0; i < NUM_LEDS; i++ ){
	// 	leds[i] = CRGB::Black;
	// }

	add_sparkles();
	FastLED.show(); //display this frame
	FastLED.delay(2 * 1000 / FPS);
}

void off(uint8_t secs) {
	for(uint16_t i = 0; i < NUM_LEDS; i++ ){
		leds[i] = CRGB::Black;
	}

	FastLED.show();
	FastLED.delay(1000 * secs);
}

void test_correction(){
	show_rgb();
	FastLED.setCorrection( TypicalLEDStrip );	
	FastLED.delay(4*1000);
	off(4);
	show_rgb();
	FastLED.setCorrection( UncorrectedColor );
	FastLED.delay(4*1000);
	off(4);
}

void test_brightness(){
	show_rgb();
	for(uint16_t i = 0; i < 255; i++){
		FastLED.setBrightness( i );
		FastLED.show(); // display this frame
		FastLED.delay(1000 / FPS);
	}
}

#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3

void test_temperature(){
	// draw a generic, no-name rainbow
	static uint8_t starthue = 0;

	for(uint8_t col = 0; col < 10; col++){		
		if(col%2 == 0) {
			fill_rainbow( &leds[XY(col,0)], HEIGHT, starthue, 20);		
		} else {
			fill_rainbow_reverse( &leds[XY(col,HEIGHT-1)], HEIGHT, starthue, 20);	
		}		
	}	
	--starthue;

	// Choose which 'color temperature' profile to enable.
	uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
	if( secs < DISPLAYTIME) {
		FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
		leds[0] = TEMPERATURE_1; // show indicator pixel
	} else {
		FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
		leds[0] = TEMPERATURE_2; // show indicator pixel
	}

	// Black out the LEDs for a few secnds between color changes
	// to let the eyes and brains adjust
	if( (secs % DISPLAYTIME) < BLACKTIME) {
		memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
	}

	FastLED.show();
	FastLED.delay(20);
}