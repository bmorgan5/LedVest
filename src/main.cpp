
#include "vest.h"
#include "fire2012.h"

CRGB leds[NUM_LEDS];

extern "C" 

// void cycle_two_led(){
// 	while(1) {
// 				// Currently only testing with 2 LEDs
// 		leds[0] = CRGB(255,0,0);
// 		leds[1] = CRGB(0,255,0);
// 		FastLED.show();
// 		delay(500);

// 		leds[0] = CRGB(0,255,0);
// 		leds[1] = CRGB(0,0, 255);
// 		FastLED.show();
// 		delay(500);

// 		leds[0] = CRGB(0,0,255);
// 		leds[1] = CRGB(255,0,0);
// 		FastLED.show();
// 		delay(500);
// 	}
// }

int main(void)
{

	setup_fire();

	while(1){
		loop_fire();
	}

	
	// delay(2000);

	// FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	// // FastLED.setBrightness(CRGB(255,255,255));

	// pinMode(13, OUTPUT);
	// digitalWriteFast(13, HIGH);

	// leds[0] = CRGB(255,0,0);
	// leds[1] = CRGB(0,255,0);
	// leds[2] = CRGB(0,0,255);
	// leds[3] = CRGB::White;
	// FastLED.show();

	// while(1) {
	// 	delay(100000);
	// }

	// cycle_two_led();

	// while (1) {
		// leds[0] = CRGB(255,255,255); 
		// leds[1] = CRGB(255,255,255);
		// leds[2] = CRGB(0,255,0);
		// leds[3] = CRGB(0,0,255);
		// leds[4] = CRGB(0,0,255);
		// leds[5] = CRGB(0,0,255);
		// FastLED.show();
		// delay(1000);





		// digitalWriteFast(13, HIGH);
		// delay(500);
		// digitalWriteFast(13, LOW);
		// delay(500);
	// }
}
