#include <stdio.h>
#include <jpeglib.h>    // Jpeg process lib
#include <setjmp.h>     // Error recovery

int main (void) {
    // Struct contains JPEG compression parameters + pointers to working space
    struct jpeg_compress_struct cinfo;
    // Struct holds JPEG error handlers
    struct jpeg_error_mgr jerr;
    
    // Set error handler
    cinfo.err = jpeg_std_error(&jerr);
    // Initialise JPEG compression object
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen("sunset_tree_alter.jpg", "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

    

    return 0;
}
