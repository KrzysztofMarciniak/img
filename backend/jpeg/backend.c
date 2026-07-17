#include "backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <jpeglib.h>


Image *
jpeg_load(const char *path)
{
    FILE *fp = fopen(path, "rb");

    if (!fp){
        return NULL;
	}

	unsigned char sig[2];
	if (fread(sig, 1, 2, fp) != 2) {
    		fclose(fp);
    	return NULL;
	}

	if (sig[0] != 0xff || sig[1] != 0xd8) {
    		fclose(fp);
    	return NULL;
	}
rewind(fp);


    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;


    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, fp);


    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return NULL;
    }


    cinfo.out_color_space = JCS_RGB;

    jpeg_start_decompress(&cinfo);


    int width = cinfo.output_width;
    int height = cinfo.output_height;


    Image *img = calloc(1, sizeof(*img));

    if (!img) {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return NULL;
    }


    img->width = width;
    img->height = height;

    img->rgba = malloc(width * height * 4);

    if (!img->rgba) {
        free(img);
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return NULL;
    }


    unsigned char *row = malloc(width * 3);

    if (!row) {
        free(img->rgba);
        free(img);
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return NULL;
    }


    while (cinfo.output_scanline < cinfo.output_height) {

        unsigned char *rowptr[1];
        rowptr[0] = row;

        jpeg_read_scanlines(&cinfo, rowptr, 1);

        int y = cinfo.output_scanline - 1;


        for (int x = 0; x < width; x++) {

            img->rgba[(y * width + x) * 4 + 0] =
                row[x * 3 + 0];

            img->rgba[(y * width + x) * 4 + 1] =
                row[x * 3 + 1];

            img->rgba[(y * width + x) * 4 + 2] =
                row[x * 3 + 2];

            img->rgba[(y * width + x) * 4 + 3] =
                255;
        }
    }


    free(row);

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(fp);

    return img;
}

