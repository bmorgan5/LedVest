#include "vest.h"
#include "fire2012.h"
#include "colors.h"
#include "gifs.h"

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
	delay(2000); // sanity delay
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setCorrection( TypicalLEDStrip );
	FastLED.setBrightness( BRIGHTNESS );
}

int main(void)
{
	pinMode(13, OUTPUT);
	digitalWriteFast(13, HIGH);

	setup();


	while(1){
		// nyan_cat();
		off(1);
		// loop_fire();
		// test_matrix();
		// test_brightness();
		// test_correction();
		// test_temperature();
		// test_sparkles();
	}
}
	
	
