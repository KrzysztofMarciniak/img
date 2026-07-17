#include "backend.h"

#include <png.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Image *
png_load(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
        return NULL;

    unsigned char sig[8];

    if (fread(sig, 1, 8, fp) != 8) {
        fclose(fp);
        return NULL;
    }

    if (png_sig_cmp(sig, 0, 8)) {
        fclose(fp);
        return NULL;
    }


    png_structp png =
        png_create_read_struct(
            PNG_LIBPNG_VER_STRING,
            NULL,
            NULL,
            NULL);

    if (!png) {
        fclose(fp);
        return NULL;
    }


    png_infop info =
        png_create_info_struct(png);

    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return NULL;
    }


    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return NULL;
    }


    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);

    png_read_info(png, info);


    int width  = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);

    int color = png_get_color_type(png, info);
    int depth = png_get_bit_depth(png, info);


    /*
     * Normalize everything to RGBA8
     */

    if (depth == 16)
        png_set_strip_16(png);

    if (color == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    if (color == PNG_COLOR_TYPE_GRAY && depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    if (color == PNG_COLOR_TYPE_RGB ||
        color == PNG_COLOR_TYPE_GRAY)
        png_set_filler(png, 0xff, PNG_FILLER_AFTER);

    if (color == PNG_COLOR_TYPE_GRAY ||
        color == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);


    png_read_update_info(png, info);


    Image *img = calloc(1, sizeof(*img));

    if (!img) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return NULL;
    }


    img->width = width;
    img->height = height;

    img->rgba = malloc(width * height * 4);

    if (!img->rgba) {
        free(img);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return NULL;
    }


    png_bytep rows[height];

    for (int y = 0; y < height; y++)
        rows[y] = img->rgba + y * width * 4;


    png_read_image(png, rows);


    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);


    return img;
}

