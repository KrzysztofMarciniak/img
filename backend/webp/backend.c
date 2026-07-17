#include "backend.h"

#include <webp/decode.h>

#include <stdio.h>
#include <stdlib.h>


Image *
webp_load(const char *path)
{
    FILE *fp = fopen(path, "rb");

    if (!fp)
        return NULL;


    fseek(fp, 0, SEEK_END);

    long size = ftell(fp);

    if (size <= 0) {
        fclose(fp);
        return NULL;
    }

    rewind(fp);


    uint8_t *data = malloc(size);

    if (!data) {
        fclose(fp);
        return NULL;
    }


    if (fread(data, 1, size, fp) != (size_t)size) {
        free(data);
        fclose(fp);
        return NULL;
    }

    fclose(fp);


    int width;
    int height;


    uint8_t *rgba =
        WebPDecodeRGBA(
            data,
            size,
            &width,
            &height);


    free(data);


    if (!rgba)
        return NULL;


    Image *img = calloc(1, sizeof(*img));

    if (!img) {
        WebPFree(rgba);
        return NULL;
    }


    img->width = width;
    img->height = height;
    img->rgba = rgba;


    return img;
}

