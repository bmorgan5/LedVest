#include "vest.h"

extern CRGB leds[NUM_LEDS];

// This function will return the right 'led index number' for 
// a given set of X and Y coordinates with (0,0) being the top
// left corner of the matrix.
// IT DOES NOT CHECK THE COORDINATE BOUNDARIES.  
// That's up to you.  Don't pass it bogus values.
uint16_t XY( uint8_t x, uint8_t y)
{
    uint16_t i;

    // Columns are reversed
    uint8_t reverseX = (WIDTH - 1) - x;
    if(reverseX & 0x01) {
      i = (reverseX * HEIGHT) + y;
    } else {    
      // Even columns run backwards
      uint8_t reverseY = (HEIGHT - 1) - y;
      i = (reverseX * HEIGHT) + reverseY;
    }
    return i;
}

// This function corrects snaked column directions
uint16_t rasterToMatrix(uint16_t i)
{
  uint8_t y = i / HEIGHT;
  uint8_t x = i % HEIGHT;

  return XY(x,y);
}

uint16_t colTop(uint8_t col)
{
  return XY(col, 0);
}

uint16_t colBottom(uint8_t col)
{
  return XY(col, HEIGHT-1);
}

uint16_t rowLeft(uint8_t row)
{
  return XY(0, row);
}
uint16_t rowRight(uint8_t row)
{
  return XY(WIDTH-1, row);
}

void test_matrix(){
  // Fire2012(); // run simulation frame
  for(int i = 0; i < WIDTH; i++){
    leds[XY(i,i)] = CRGB::Blue;
  }

  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}


