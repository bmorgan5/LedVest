#include "colors.h"
#include "gifs.h"
#include "vest.h"

// #include "fire2012.h"

// #include "gifs.h"



CRGB leds[NUM_LEDS];

extern "C" 

void off(uint8_t secs) {
	for(uint16_t i = 0; i < NUM_LEDS; i++ ){
		leds[i] = CRGB::Black;
	}

	FastLED.show();
	FastLED.delay(1000 * secs);
}

void setup() {
	pinMode(13, OUTPUT);
	digitalWriteFast(13, HIGH);

	delay(2000); // sanity delay

	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setCorrection( TypicalLEDStrip );
	FastLED.setBrightness( BRIGHTNESS );
}




int main(void)
{
	setup();

	while(1){
		// off(1);

		bm_logo();
		// test_rainbow_palette();

		// nyan_cat();		
		// loop_fire();
		// test_matrix();
		// test_brightness();
		// test_correction();
		// test_temperature();
		// test_sparkles();
	}
}
	
	
