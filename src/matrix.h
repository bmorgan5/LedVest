#ifndef MATRIX_H
#define MATRIX_H

// Takes (X,Y) coordinates are returns the index
// in the led strip for the vest matrix
uint16_t XY(uint8_t x, uint8_t y);

// Takes the Rasterbits index from a gif and
// returns the index in the led strip for the
// vest matrix
uint16_t R(uint16_t i);

uint16_t colTop(uint8_t col);
uint16_t colBottom(uint8_t col);

uint16_t rowLeft(uint8_t row);
uint16_t rowRight(uint8_t row);

void test_matrix();

#endif /* MATRIX_H */