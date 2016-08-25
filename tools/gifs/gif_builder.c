/*************************************************************************
* gif_builder is a tool to build and verify gifs for the led_vest display
* Benjamin Morgan, 2016
* github.com/bmorgan5/led_vest
* requies giflib-5.1.4 at least
*************************************************************************/
#include "gif_builder.h"
#include <sys/stat.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define APPLY_DIMMING(X) (X)
#define HSV_SECTION_6 (0x20)
#define HSV_SECTION_3 (0x40)

void hsv2rgb_raw_C (const struct CHSV* hsv, struct CRGB* rgb)
{
    // Convert hue, saturation and brightness ( HSV/HSB ) to RGB
    // "Dimming" is used on saturation and brightness to make
    // the output more visually linear.

    // Apply dimming curves
    uint8_t value = APPLY_DIMMING( hsv->val);
    uint8_t saturation = hsv->sat;

    // The brightness floor is minimum number that all of
    // R, G, and B will be set to.
    uint8_t invsat = APPLY_DIMMING( 255 - saturation);
    uint8_t brightness_floor = (value * invsat) / 256;

    // The color amplitude is the maximum amount of R, G, and B
    // that will be added on top of the brightness_floor to
    // create the specific hue desired.
    uint8_t color_amplitude = value - brightness_floor;

    // Figure out which section of the hue wheel we're in,
    // and how far offset we are withing that section
    uint8_t section = hsv->hue / HSV_SECTION_3; // 0..2
    uint8_t offset = hsv->hue % HSV_SECTION_3;  // 0..63

    uint8_t rampup = offset; // 0..63
    uint8_t rampdown = (HSV_SECTION_3 - 1) - offset; // 63..0

    // We now scale rampup and rampdown to a 0-255 range -- at least
    // in theory, but here's where architecture-specific decsions
    // come in to play:
    // To scale them up to 0-255, we'd want to multiply by 4.
    // But in the very next step, we multiply the ramps by other
    // values and then divide the resulting product by 256.
    // So which is faster?
    //   ((ramp * 4) * othervalue) / 256
    // or
    //   ((ramp    ) * othervalue) /  64
    // It depends on your processor architecture.
    // On 8-bit AVR, the "/ 256" is just a one-cycle register move,
    // but the "/ 64" might be a multicycle shift process. So on AVR
    // it's faster do multiply the ramp values by four, and then
    // divide by 256.
    // On ARM, the "/ 256" and "/ 64" are one cycle each, so it's
    // faster to NOT multiply the ramp values by four, and just to
    // divide the resulting product by 64 (instead of 256).
    // Moral of the story: trust your profiler, not your insticts.

    // Since there's an AVR assembly version elsewhere, we'll
    // assume what we're on an architecture where any number of
    // bit shifts has roughly the same cost, and we'll remove the
    // redundant math at the source level:

    //  // scale up to 255 range
    //  //rampup *= 4; // 0..252
    //  //rampdown *= 4; // 0..252

    // compute color-amplitude-scaled-down versions of rampup and rampdown
    uint8_t rampup_amp_adj   = (rampup   * color_amplitude) / (256 / 4);
    uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

    // add brightness_floor offset to everything
    uint8_t rampup_adj_with_floor   = rampup_amp_adj   + brightness_floor;
    uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;


    if( section ) {
        if( section == 1) {
            // section 1: 0x40..0x7F
            rgb->r = brightness_floor;
            rgb->g = rampdown_adj_with_floor;
            rgb->b = rampup_adj_with_floor;
        } else {
            // section 2; 0x80..0xBF
            rgb->r = rampup_adj_with_floor;
            rgb->g = brightness_floor;
            rgb->b = rampdown_adj_with_floor;
        }
    } else {
        // section 0: 0x00..0x3F
        rgb->r = rampdown_adj_with_floor;
        rgb->g = rampup_adj_with_floor;
        rgb->b = brightness_floor;
    }
}
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

GifFileType* slurpGifLiteral(GifByteType* gif_bytes)
{
	int	ErrorCode;
    GifFileType *GifFileIn;

	GifLiteral gifLiteral = {
		.bytes = gif_bytes,
		.size = sizeof(gif_bytes),
		.cursor = 0
	};

    if ((GifFileIn = DGifOpen(&gifLiteral, read_gif_literal, &ErrorCode)) == NULL) {
    	PrintGifError(ErrorCode);
    	exit(EXIT_FAILURE);
    }

    if (DGifSlurp(GifFileIn) == GIF_ERROR) {
    	PrintGifError(GifFileIn->Error);
    	exit(EXIT_FAILURE);
    }

    return GifFileIn;
}

int build_nyan_cat()
{
	int	i, ErrorCode;
    GifFileType *GifFileIn, *GifFileOut = (GifFileType *)NULL;

    GifFileIn = slurpGifLiteral(nyan_cat_bytes);
	
    if ((GifFileOut = EGifOpenFileName("nyan_cat_test.gif", false, &ErrorCode)) == NULL) {
    	PrintGifError(ErrorCode);
    	exit(EXIT_FAILURE);
    }

    /*
     * Your operations on in-core structures go here.  
     * This code just copies the header and each image from the incoming file.
     */
    GifFileOut->SWidth = GifFileIn->SWidth;
    GifFileOut->SHeight = GifFileIn->SHeight;
    GifFileOut->SColorResolution = GifFileIn->SColorResolution;
    GifFileOut->SBackGroundColor = GifFileIn->SBackGroundColor;
    if (GifFileIn->SColorMap) {
		GifFileOut->SColorMap = GifMakeMapObject(
				   GifFileIn->SColorMap->ColorCount,
				   GifFileIn->SColorMap->Colors);
    } else {
		GifFileOut->SColorMap = NULL;
    }

    for (i = 0; i < GifFileIn->ImageCount; i++)
		(void) GifMakeSavedImage(GifFileOut, &GifFileIn->SavedImages[i]);

    /*
     * Note: don't do DGifCloseFile early, as this will
     * deallocate all the memory containing the GIF data!
     *
     * Further note: EGifSpew() doesn't try t[3]o validity-check any of this
     * data; it's *your* responsibility to keep your changes consistent.
     * Caveat hacker!
     */
    if (EGifSpew(GifFileOut) == GIF_ERROR)
		PrintGifError(GifFileOut->Error);

    if (DGifCloseFile(GifFileIn, &ErrorCode) == GIF_ERROR)
		PrintGifError(ErrorCode);

    return 0;

}

uint8_t rainbow_colors[256][3] = {
    {0xE2, 0x00, 0x00}, 
    {0xE0, 0x01, 0x00},
    {0xDD, 0x04, 0x00},
    {0xDB, 0x07, 0x00},
    {0xD9, 0x08, 0x00},
    {0xD6, 0x0B, 0x00},
    {0xD3, 0x0E, 0x00},
    {0xD2, 0x0F, 0x00},
    {0xCF, 0x12, 0x00},
    {0xCC, 0x15, 0x00},
    {0xCB, 0x17, 0x00},
    {0xC8, 0x19, 0x00},
    {0xC5, 0x1C, 0x00},
    {0xC3, 0x1E, 0x00},
    {0xC1, 0x20, 0x00},
    {0xBE, 0x23, 0x00},
    {0xBB, 0x26, 0x00},
    {0xBA, 0x27, 0x00},
    {0xB7, 0x2A, 0x00},
    {0xB4, 0x2D, 0x00},
    {0xB3, 0x2E, 0x00},
    {0xB0, 0x31, 0x00},
    {0xAD, 0x34, 0x00},
    {0xAC, 0x36, 0x00},
    {0xA9, 0x38, 0x00},
    {0xA6, 0x3B, 0x00},
    {0xA4, 0x3D, 0x00},
    {0xA2, 0x3F, 0x00},
    {0x9F, 0x42, 0x00},
    {0x9D, 0x44, 0x00},
    {0x9B, 0x46, 0x00},
    {0x98, 0x49, 0x00},
    {0x97, 0x4B, 0x00},
    {0x97, 0x4D, 0x00},
    {0x97, 0x4F, 0x00},
    {0x97, 0x52, 0x00},
    {0x97, 0x54, 0x00},
    {0x97, 0x56, 0x00},
    {0x97, 0x59, 0x00},
    {0x97, 0x5B, 0x00},
    {0x97, 0x5D, 0x00},
    {0x97, 0x60, 0x00},
    {0x97, 0x62, 0x00},
    {0x97, 0x65, 0x00},
    {0x97, 0x67, 0x00},
    {0x97, 0x69, 0x00},
    {0x97, 0x6C, 0x00},
    {0x97, 0x6E, 0x00},
    {0x97, 0x71, 0x00},
    {0x97, 0x73, 0x00},
    {0x97, 0x75, 0x00},
    {0x97, 0x78, 0x00},
    {0x97, 0x7A, 0x00},
    {0x97, 0x7D, 0x00},
    {0x97, 0x7F, 0x00},
    {0x97, 0x81, 0x00},
    {0x97, 0x84, 0x00},
    {0x97, 0x86, 0x00},
    {0x97, 0x88, 0x00},
    {0x97, 0x8B, 0x00},
    {0x97, 0x8D, 0x00},
    {0x97, 0x8F, 0x00},
    {0x97, 0x92, 0x00},
    {0x97, 0x94, 0x00},
    {0x97, 0x96, 0x00},
    {0x93, 0x98, 0x00},
    {0x8E, 0x9B, 0x00},
    {0x89, 0x9D, 0x00},
    {0x85, 0x9F, 0x00},
    {0x80, 0xA2, 0x00},
    {0x7B, 0xA4, 0x00},
    {0x76, 0xA6, 0x00},
    {0x72, 0xA9, 0x00},
    {0x6D, 0xAC, 0x00},
    {0x68, 0xAD, 0x00},
    {0x64, 0xB0, 0x00},
    {0x5E, 0xB3, 0x00},
    {0x5A, 0xB4, 0x00},
    {0x56, 0xB7, 0x00},
    {0x50, 0xBA, 0x00},
    {0x4C, 0xBC, 0x00},
    {0x47, 0xBE, 0x00},
    {0x42, 0xC1, 0x00},
    {0x3E, 0xC3, 0x00},
    {0x39, 0xC5, 0x00},
    {0x34, 0xC8, 0x00},
    {0x2F, 0xCB, 0x00},
    {0x2B, 0xCC, 0x00},
    {0x26, 0xCF, 0x00},
    {0x21, 0xD2, 0x00},
    {0x1D, 0xD3, 0x00},
    {0x17, 0xD6, 0x00},
    {0x13, 0xD9, 0x00},
    {0x0F, 0xDB, 0x00},
    {0x09, 0xDD, 0x00},
    {0x05, 0xE0, 0x00},
    {0x00, 0xE2, 0x00},
    {0x00, 0xE0, 0x01},
    {0x00, 0xDD, 0x04},
    {0x00, 0xDB, 0x07},
    {0x00, 0xD9, 0x08},
    {0x00, 0xD6, 0x0B},
    {0x00, 0xD3, 0x0E},
    {0x00, 0xD2, 0x0F},
    {0x00, 0xCF, 0x12},
    {0x00, 0xCC, 0x15},
    {0x00, 0xCB, 0x17},
    {0x00, 0xC8, 0x19},
    {0x00, 0xC5, 0x1C},
    {0x00, 0xC3, 0x1E},
    {0x00, 0xC1, 0x20},
    {0x00, 0xBE, 0x23},
    {0x00, 0xBB, 0x26},
    {0x00, 0xBA, 0x27},
    {0x00, 0xB7, 0x2A},
    {0x00, 0xB4, 0x2D},
    {0x00, 0xB3, 0x2E},
    {0x00, 0xB0, 0x31},
    {0x00, 0xAD, 0x34},
    {0x00, 0xAC, 0x36},
    {0x00, 0xA9, 0x38},
    {0x00, 0xA6, 0x3B},
    {0x00, 0xA4, 0x3D},
    {0x00, 0xA2, 0x3F},
    {0x00, 0x9F, 0x42},
    {0x00, 0x9D, 0x44},
    {0x00, 0x9B, 0x46},
    {0x00, 0x98, 0x49},
    {0x00, 0x97, 0x4B},
    {0x00, 0x93, 0x4F},
    {0x00, 0x8E, 0x54},
    {0x00, 0x89, 0x59},
    {0x00, 0x85, 0x5D},
    {0x00, 0x80, 0x62},
    {0x00, 0x7B, 0x67},
    {0x00, 0x76, 0x6C},
    {0x00, 0x72, 0x70},
    {0x00, 0x6D, 0x75},
    {0x00, 0x68, 0x7A},
    {0x00, 0x64, 0x7E},
    {0x00, 0x5E, 0x84},
    {0x00, 0x5A, 0x88},
    {0x00, 0x56, 0x8C},
    {0x00, 0x50, 0x92},
    {0x00, 0x4C, 0x96},
    {0x00, 0x47, 0x9B},
    {0x00, 0x42, 0xA0},
    {0x00, 0x3E, 0xA4},
    {0x00, 0x39, 0xA9},
    {0x00, 0x34, 0xAE},
    {0x00, 0x2F, 0xB3},
    {0x00, 0x2B, 0xB7},
    {0x00, 0x26, 0xBC},
    {0x00, 0x21, 0xC1},
    {0x00, 0x1D, 0xC5},
    {0x00, 0x17, 0xCB},
    {0x00, 0x13, 0xCF},
    {0x00, 0x0F, 0xD3},
    {0x00, 0x09, 0xD9},
    {0x00, 0x05, 0xDD},
    {0x00, 0x00, 0xE2},
    {0x01, 0x00, 0xE0},
    {0x04, 0x00, 0xDD},
    {0x07, 0x00, 0xDB},
    {0x08, 0x00, 0xD9},
    {0x0B, 0x00, 0xD6},
    {0x0E, 0x00, 0xD3},
    {0x0F, 0x00, 0xD2},
    {0x12, 0x00, 0xCF},
    {0x15, 0x00, 0xCC},
    {0x17, 0x00, 0xCB},
    {0x19, 0x00, 0xC8},
    {0x1C, 0x00, 0xC5},
    {0x1E, 0x00, 0xC3},
    {0x20, 0x00, 0xC1},
    {0x23, 0x00, 0xBE},
    {0x26, 0x00, 0xBB},
    {0x27, 0x00, 0xBA},
    {0x2A, 0x00, 0xB7},
    {0x2D, 0x00, 0xB4},
    {0x2E, 0x00, 0xB3},
    {0x31, 0x00, 0xB0},
    {0x34, 0x00, 0xAD},
    {0x36, 0x00, 0xAC},
    {0x38, 0x00, 0xA9},
    {0x3B, 0x00, 0xA6},
    {0x3D, 0x00, 0xA4},
    {0x3F, 0x00, 0xA2},
    {0x42, 0x00, 0x9F},
    {0x44, 0x00, 0x9D},
    {0x46, 0x00, 0x9B},
    {0x49, 0x00, 0x98},
    {0x4B, 0x00, 0x97},
    {0x4D, 0x00, 0x95},
    {0x4F, 0x00, 0x93},
    {0x52, 0x00, 0x90},
    {0x54, 0x00, 0x8E},
    {0x56, 0x00, 0x8C},
    {0x59, 0x00, 0x89},
    {0x5B, 0x00, 0x87},
    {0x5D, 0x00, 0x85},
    {0x60, 0x00, 0x82},
    {0x62, 0x00, 0x80},
    {0x65, 0x00, 0x7D},
    {0x67, 0x00, 0x7B},
    {0x69, 0x00, 0x79},
    {0x6C, 0x00, 0x76},
    {0x6E, 0x00, 0x74},
    {0x71, 0x00, 0x71},
    {0x73, 0x00, 0x6F},
    {0x75, 0x00, 0x6D},
    {0x78, 0x00, 0x6A},
    {0x7A, 0x00, 0x68},
    {0x7D, 0x00, 0x65},
    {0x7F, 0x00, 0x63},
    {0x81, 0x00, 0x61},
    {0x84, 0x00, 0x5E},
    {0x86, 0x00, 0x5C},
    {0x88, 0x00, 0x5A},
    {0x8B, 0x00, 0x57},
    {0x8D, 0x00, 0x55},
    {0x8F, 0x00, 0x53},
    {0x92, 0x00, 0x50},
    {0x94, 0x00, 0x4E},
    {0x96, 0x00, 0x4B},
    {0x98, 0x00, 0x49},
    {0x9B, 0x00, 0x46},
    {0x9D, 0x00, 0x44},
    {0x9F, 0x00, 0x42},
    {0xA2, 0x00, 0x3F},
    {0xA4, 0x00, 0x3D},
    {0xA6, 0x00, 0x3B},
    {0xA9, 0x00, 0x38},
    {0xAC, 0x00, 0x36},
    {0xAD, 0x00, 0x34},
    {0xB0, 0x00, 0x31},
    {0xB3, 0x00, 0x2E},
    {0xB4, 0x00, 0x2D},
    {0xB7, 0x00, 0x2A},
    {0xBA, 0x00, 0x27},
    {0xBC, 0x00, 0x25},
    {0xBE, 0x00, 0x23},
    {0xC1, 0x00, 0x20},
    {0xC3, 0x00, 0x1E},
    {0xC5, 0x00, 0x1C},
    {0xC8, 0x00, 0x19},
    {0xCB, 0x00, 0x17},
    {0xCC, 0x00, 0x15},
    {0xCF, 0x00, 0x12},
    {0xD2, 0x00, 0x0F},
    {0xD3, 0x00, 0x0E},
    {0xD6, 0x00, 0x0B},
    {0xD9, 0x00, 0x08},
    {0xDB, 0x00, 0x07},
    {0xFF, 0xFF, 0xFF},
    {0x00, 0x00, 0x00}
};

uint8_t riley_rainbow[256][3] = {
    { 255, 0  , 0   },
    { 253, 2  , 0   },
    { 250, 5  , 0   },
    { 248, 7  , 0   },
    { 245, 10 , 0   },
    { 242, 13 , 0   },
    { 240, 15 , 0   },
    { 237, 18 , 0   },
    { 234, 21 , 0   },
    { 232, 23 , 0   },
    { 229, 26 , 0   },
    { 226, 29 , 0   },
    { 224, 31 , 0   },
    { 221, 34 , 0   },
    { 218, 37 , 0   },
    { 216, 39 , 0   },
    { 213, 42 , 0   },
    { 210, 45 , 0   },
    { 208, 47 , 0   },
    { 205, 50 , 0   },
    { 202, 53 , 0   },
    { 200, 55 , 0   },
    { 197, 58 , 0   },
    { 194, 61 , 0   },
    { 192, 63 , 0   },
    { 189, 66 , 0   },
    { 186, 69 , 0   },
    { 184, 71 , 0   },
    { 181, 74 , 0   },
    { 178, 77 , 0   },
    { 176, 79 , 0   },
    { 173, 82 , 0   },
    { 171, 85 , 0   },
    { 171, 87 , 0   },
    { 171, 90 , 0   },
    { 171, 92 , 0   },
    { 171, 95 , 0   },
    { 171, 98 , 0   },
    { 171, 100, 0   },
    { 171, 103, 0   },
    { 171, 106, 0   },
    { 171, 108, 0   },
    { 171, 111, 0   },
    { 171, 114, 0   },
    { 171, 116, 0   },
    { 171, 119, 0   },
    { 171, 122, 0   },
    { 171, 124, 0   },
    { 171, 127, 0   },
    { 171, 130, 0   },
    { 171, 132, 0   },
    { 171, 135, 0   },
    { 171, 138, 0   },
    { 171, 140, 0   },
    { 171, 143, 0   },
    { 171, 146, 0   },
    { 171, 148, 0   },
    { 171, 151, 0   },
    { 171, 154, 0   },
    { 171, 156, 0   },
    { 171, 159, 0   },
    { 171, 162, 0   },
    { 171, 164, 0   },
    { 171, 167, 0   },
    { 171, 171, 0   },
    { 166, 173, 0   },
    { 161, 176, 0   },
    { 156, 178, 0   },
    { 150, 181, 0   },
    { 145, 184, 0   },
    { 140, 186, 0   },
    { 134, 189, 0   },
    { 129, 192, 0   },
    { 124, 194, 0   },
    { 118, 197, 0   },
    { 113, 200, 0   },
    { 108, 202, 0   },
    { 102, 205, 0   },
    { 97 , 208, 0   },
    { 92 , 210, 0   },
    { 86 , 213, 0   },
    { 81 , 216, 0   },
    { 76 , 218, 0   },
    { 71 , 221, 0   },
    { 65 , 224, 0   },
    { 60 , 226, 0   },
    { 55 , 229, 0   },
    { 49 , 232, 0   },
    { 44 , 234, 0   },
    { 39 , 237, 0   },
    { 33 , 240, 0   },
    { 28 , 242, 0   },
    { 23 , 245, 0   },
    { 17 , 248, 0   },
    { 12 , 250, 0   },
    { 7  , 253, 0   },
    { 0  , 255, 0   },
    { 0  , 253, 2   },
    { 0  , 250, 5   },
    { 0  , 248, 7   },
    { 0  , 245, 10  },
    { 0  , 242, 13  },
    { 0  , 240, 15  },
    { 0  , 237, 18  },
    { 0  , 234, 21  },
    { 0  , 232, 23  },
    { 0  , 229, 26  },
    { 0  , 226, 29  },
    { 0  , 224, 31  },
    { 0  , 221, 34  },
    { 0  , 218, 37  },
    { 0  , 216, 39  },
    { 0  , 213, 42  },
    { 0  , 210, 45  },
    { 0  , 208, 47  },
    { 0  , 205, 50  },
    { 0  , 202, 53  },
    { 0  , 200, 55  },
    { 0  , 197, 58  },
    { 0  , 194, 61  },
    { 0  , 192, 63  },
    { 0  , 189, 66  },
    { 0  , 186, 69  },
    { 0  , 184, 71  },
    { 0  , 181, 74  },
    { 0  , 178, 77  },
    { 0  , 176, 79  },
    { 0  , 173, 82  },
    { 0  , 171, 85  },
    { 0  , 166, 90  },
    { 0  , 161, 95  },
    { 0  , 156, 100 },
    { 0  , 150, 106 },
    { 0  , 145, 111 },
    { 0  , 140, 116 },
    { 0  , 134, 122 },
    { 0  , 129, 127 },
    { 0  , 124, 132 },
    { 0  , 118, 138 },
    { 0  , 113, 143 },
    { 0  , 108, 148 },
    { 0  , 102, 154 },
    { 0  , 97 , 159 },
    { 0  , 92 , 164 },
    { 0  , 86 , 170 },
    { 0  , 81 , 175 },
    { 0  , 76 , 180 },
    { 0  , 71 , 185 },
    { 0  , 65 , 191 },
    { 0  , 60 , 196 },
    { 0  , 55 , 201 },
    { 0  , 49 , 207 },
    { 0  , 44 , 212 },
    { 0  , 39 , 217 },
    { 0  , 33 , 223 },
    { 0  , 28 , 228 },
    { 0  , 23 , 233 },
    { 0  , 17 , 239 },
    { 0  , 12 , 244 },
    { 0  , 7  , 249 },
    { 0  , 0  , 255 },
    { 2  , 0  , 253 },
    { 5  , 0  , 250 },
    { 7  , 0  , 248 },
    { 10 , 0  , 245 },
    { 13 , 0  , 242 },
    { 15 , 0  , 240 },
    { 18 , 0  , 237 },
    { 21 , 0  , 234 },
    { 23 , 0  , 232 },
    { 26 , 0  , 229 },
    { 29 , 0  , 226 },
    { 31 , 0  , 224 },
    { 34 , 0  , 221 },
    { 37 , 0  , 218 },
    { 39 , 0  , 216 },
    { 42 , 0  , 213 },
    { 45 , 0  , 210 },
    { 47 , 0  , 208 },
    { 50 , 0  , 205 },
    { 53 , 0  , 202 },
    { 55 , 0  , 200 },
    { 58 , 0  , 197 },
    { 61 , 0  , 194 },
    { 63 , 0  , 192 },
    { 66 , 0  , 189 },
    { 69 , 0  , 186 },
    { 71 , 0  , 184 },
    { 74 , 0  , 181 },
    { 77 , 0  , 178 },
    { 79 , 0  , 176 },
    { 82 , 0  , 173 },
    { 85 , 0  , 171 },
    { 87 , 0  , 169 },
    { 90 , 0  , 166 },
    { 92 , 0  , 164 },
    { 95 , 0  , 161 },
    { 98 , 0  , 158 },
    { 100, 0  , 156 },
    { 103, 0  , 153 },
    { 106, 0  , 150 },
    { 108, 0  , 148 },
    { 111, 0  , 145 },
    { 114, 0  , 142 },
    { 116, 0  , 140 },
    { 119, 0  , 137 },
    { 122, 0  , 134 },
    { 124, 0  , 132 },
    { 127, 0  , 129 },
    { 130, 0  , 126 },
    { 132, 0  , 124 },
    { 135, 0  , 121 },
    { 138, 0  , 118 },
    { 140, 0  , 116 },
    { 143, 0  , 113 },
    { 146, 0  , 110 },
    { 148, 0  , 108 },
    { 151, 0  , 105 },
    { 154, 0  , 102 },
    { 156, 0  , 100 },
    { 159, 0  , 97  },
    { 162, 0  , 94  },
    { 164, 0  , 92  },
    { 167, 0  , 89  },
    { 171, 0  , 85  },
    { 173, 0  , 83  },
    { 176, 0  , 80  },
    { 178, 0  , 78  },
    { 181, 0  , 75  },
    { 184, 0  , 72  },
    { 186, 0  , 70  },
    { 189, 0  , 67  },
    { 192, 0  , 64  },
    { 194, 0  , 62  },
    { 197, 0  , 59  },
    { 200, 0  , 56  },
    { 202, 0  , 54  },
    { 205, 0  , 51  },
    { 208, 0  , 48  },
    { 210, 0  , 46  },
    { 213, 0  , 43  },
    { 216, 0  , 40  },
    { 218, 0  , 38  },
    { 221, 0  , 35  },
    { 224, 0  , 32  },
    { 226, 0  , 30  },
    { 229, 0  , 27  },
    { 232, 0  , 24  },
    { 234, 0  , 22  },
    { 237, 0  , 19  },
    { 240, 0  , 16  },
    { 242, 0  , 14  },
    { 245, 0  , 11  },
    { 248, 0  , 8   },
    { 250, 0  , 6   },
    { 253, 0  , 3   },
};

void print_gif(const char* file_name)
{
    
    for(int i = 0; i < 256; i++){
        printf("{%3d|%3d, %3d|%3d, %3d|%3d}\n", 
            rainbow_colors[i][0], riley_rainbow[i][0],
            rainbow_colors[i][1], riley_rainbow[i][1],
            rainbow_colors[i][2], riley_rainbow[i][2]);
    }

    // struct stat st;
    // stat(file_name, &st);
    // int size = st.st_size;
  
    // uint8_t bytes[size];

    // FILE* fp = fopen(file_name,"r");
    // int read_bytes = fread(bytes, 1, size, fp);
    // if(read_bytes != size)
    //     return;
    
    // for(int i = 0; i < size; i++)
    // {
    //     printf("0x%02x,", bytes[i]);
    //     if(i % 8 == 7) {
    //         printf("\n");
    //     }
    // }
    // printf("\n");

    // fclose(fp);   
}
// void print_gif_literal(){
//     FILE
// }

int main(int argc, char* argv[])
{
    // print_gif("man.gif");
    // print_gif("bm_logo.gif");
    print_gif("nyan_cat.gif");


	// print_color_map();

	// return build_nyan_cat();
}