#include "backend.h"

#include <stdlib.h>


#ifdef WITH_PNG
#include "png/backend.h"
#endif

#ifdef WITH_JPEG
#include "jpeg/backend.h"
#endif

#ifdef WITH_WEBP
#include "webp/backend.h"
#endif

Image *
img_load(const char *path)
{
    Image *img;

#ifdef WITH_PNG
    if ((img = png_load(path)))
        return img;
#endif

#ifdef WITH_JPEG
    if ((img = jpeg_load(path)))
        return img;
#endif

#ifdef WITH_WEBP
    if ((img = webp_load(path)))
        return img;
#endif

    return NULL;
}

void
img_free(Image *img)
{
    if (!img)
        return;

    free(img->rgba);
    free(img);
}

