#include "vest.h"
#include "matrix.h"

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