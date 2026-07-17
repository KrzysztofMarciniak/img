#ifndef BACKEND_H
#define BACKEND_H

#include <stdint.h>

typedef struct Image {
    int width;
    int height;
    uint8_t *rgba;
} Image;

Image *img_load(const char *path);
void img_free(Image *img);

#endif

