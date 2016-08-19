#ifndef GIFS_H
#define GIFS_H

#include "gif_lib.h"

typedef struct GifLiteral {
	GifByteType* bytes;
	int size;
	int cursor; // Points to the first unread byte (should be initialized to 0)
} GifLiteral;

// Every gif file needs this
int read_gif_literal(GifFileType* gifFile, GifByteType* buf, int count);

void nyan_cat();

#endif /* GIFS_H */