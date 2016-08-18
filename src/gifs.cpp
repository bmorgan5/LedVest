
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