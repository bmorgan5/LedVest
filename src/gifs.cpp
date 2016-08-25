#include "vest.h"
#include "gifs.h"

int read_gif_literal(GifFileType* gifFile, GifByteType* buf, int count)
{	

	GifLiteral* gl = (GifLiteral*) gifFile->UserData;

	int bytes_read = MIN(count, gl->size - gl->cursor);

	memcpy(buf, &gl->bytes[gl->cursor], bytes_read);

	gl->cursor += bytes_read;

	if(gl->cursor >= gl->size)
		gl->cursor = 0; //Reset cursor to beginning of file

	return bytes_read;
}

void fail_loop() {	
	while(1){	
		FastLED.show();	
		FastLED.delay(1000);
	}
}

// void display_gif(GifByteType* bytes, int size){
// 	GifLiteral gif_lit = {
// 		.bytes = bytes,
// 		.size = size,
// 		.cursor = 0
// 	};

// 	int Error;
// 	GifFileType* gifFile = DGifOpen(&gif_lit, read_gif_literal, &Error);
// 	if(gifFile == NULL){		
// 		leds[0] = CRGB::Red;
// 		fail_loop();
// 	}
// 	Error = DGifSlurp(gifFile);

// 	if(Error != GIF_OK){
// 		leds[(uint8_t)Error] = CRGB::Red;
// 		fail_loop();
// 	}

// 	GifColorType* colors = gifFile->SColorMap->Colors;
// 	uint8_t colorCount = gifFile->SColorMap->ColorCount;
	
// 	for(uint8_t i = 0; i < gifFile->ImageCount; i++){

// 		SavedImage* img = &gifFile->SavedImages[i];
// 		if(img == NULL){
// 			leds[2] = CRGB::Red;
// 			fail_loop();
// 		}

// 		uint16_t num_pixels = img->ImageDesc.Width * img->ImageDesc.Height;

// 		for(uint16_t p = 0; p < num_pixels; p++)
// 		{
// 			GifByteType color_index = img->RasterBits[p];
// 			if (color_index >= colorCount){ //Pixel is outside palette range
// 				leds[R(p)] = CRGB::Red;
// 			} else {
// 				GifColorType c = colors[img->RasterBits[p]];
// 				leds[R(p)] = CRGB(c.Red,c.Green,c.Blue);	
// 			}				
// 		}

// 		FastLED.show();			
// 		FastLED.delay(250);
// 	}
// }