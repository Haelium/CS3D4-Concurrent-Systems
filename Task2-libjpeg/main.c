#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <jpeglib.h>


// jpeglib does everything through objects called jpeg_compress_struct and jpeg_decompress_struct
// Each of these objects has a bunch of functions which take pointers to these objects as input

// jpeg_decompress_struct cheatsheet
// Constructor:		jpeg_create_decompress(jpeg_decompress_struct* <varname>);
// Destructor:		jpeg_destroy_decompress(jpeg_decompress_struct* <varname>);


int main (int argc, char *argv[]) {
	int rc, i, j;

	// Check that user has actually provided a filepath as an argument
	if (argc != 2) {
		fprintf(stderr, "USAGE: %s filename.jpg\n", argv[0]);
		return 1;
	}

	// Setup for decompression starts here

	// Variables for the source jpg
	struct stat file_info;
	unsigned long jpg_size;
	unsigned char *jpg_buffer;

	// decompressor object declarations
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// Variables for the output buffer, and how long each row is
	unsigned long bmp_size;
	unsigned char *bmp_buffer;
	int row_stride, width, height, pixel_size;


	// Load the jpeg data from a file into a memory buffer
	rc = stat(argv[1], &file_info);
	if (rc) {
		printf("ERROR: File %s, not found or could not be opened\n", argv[1]);
		return 1;	// return 1 for ERROR
	}
	jpg_size = file_info.st_size;
	jpg_buffer = (unsigned char*) malloc(jpg_size + 100);	// 100 bytes padding to avoid accidental segfault

	int fd = open(argv[1], O_RDONLY);
	i = 0;
	while (i < jpg_size) {
		rc = read(fd, jpg_buffer + i, jpg_size - i);
		i += rc;
	}
	close(fd);

	// Decompression starts here

	// Allocate a new decompress struct, with the default error handler.
	// The default error handler will exit() on pretty much any issue,
	// so it's likely you'll want to replace it or supplement it with
	// your own.
	cinfo.err = jpeg_std_error(&jerr);	
	jpeg_create_decompress(&cinfo);

	// Configure the decompressor object cinfo 
	// to read its data from jpeg_buffer 
	// jpg_size is the size of the buffer
	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

	// Have the decompressor scan the jpeg header. This won't populate
	// the cinfo struct output fields, but will indicate if the
	// jpeg is valid.
	rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1) {
		exit(EXIT_FAILURE);
	}

	// By calling jpeg_start_decompress, you populate cinfo
	// and can then allocate your output bitmap buffers for
	// each scanline.
	jpeg_start_decompress(&cinfo);
	
	width = cinfo.output_width;
	height = cinfo.output_height;
	pixel_size = cinfo.output_components;

	bmp_size = width * height * pixel_size;
	bmp_buffer = (unsigned char*) malloc(bmp_size);

	// The row_stride is the total number of bytes it takes to store an
	// entire scanline (row). 
	row_stride = width * pixel_size;

	while (cinfo.output_scanline < cinfo.output_height) {
		unsigned char *buffer_array[1];
		buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;

		jpeg_read_scanlines(&cinfo, buffer_array, 1);

	}

	// Decompression is finished, change cinfo state to reflect this
	jpeg_finish_decompress(&cinfo);
	
	// Destroy jpeg_decompress object labled cinfo and free buffer memory
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);

	// Invert colours
	for (unsigned long x = 0; x < bmp_size; x++) {
		//printf("%c ", bmp_buffer[x]);
		bmp_buffer[x] ^= 0xFF;	// Invert the colours in the image by flipping all bits
	}
	
	// Write the decompressed bitmap out to a ppm file, just to make sure 
	// it worked. 
	fd = open("output2.ppm", O_CREAT | O_WRONLY, 0666);
	char buf[1024];

	rc = sprintf(buf, "P6 %d %d 255\n", width, height);
	write(fd, buf, rc); // Write the PPM image header before data
	write(fd, bmp_buffer, bmp_size); // Write out all RGB pixel data

	close(fd);
	free(bmp_buffer);

	return 0;
}